#include "PlanificadorDeMovimiento.hpp"
#include "../dispositivos/interruptores/FinalDeCarrera.hpp"
#include "../dispositivos/motores/DRV8825Driver.hpp"
#include "../dispositivos/servo/ServoBoli.hpp"
#include "fifo/FifoWriter.hpp"
#include <cmath>
#include "../include/cargaParametros.hpp"

PlanificadorDeMovimiento::PlanificadorDeMovimiento()
    : parametros(Parametros::getInstance()) {

    aceleracion = parametros.getAceleracion();
    velocidadPasosPorSegundo = parametros.getVelocidadMax();

    // Calculamos la distancia en pasos de la rampa de aceleración
    pasosParada = (velocidadPasosPorSegundo * velocidadPasosPorSegundo) / (2 * aceleracion);

    std::cout << "[PlanificadorDeMovimiento] : aceleracion: " << aceleracion
        << ", velocidadPasosPorSegundo: " << velocidadPasosPorSegundo
        << ", pasosParada: " << pasosParada
        << std::endl;

}

void PlanificadorDeMovimiento::setMotores(MotorDriver& motorX,
                                          MotorDriver& motorY) {
    p_motorX = &motorX;
    p_motorY = &motorY;
}

void PlanificadorDeMovimiento::setServoBoli(ServoBoli& servoBoli) {
    p_servoBoli = &servoBoli;
}


void PlanificadorDeMovimiento::setFinalesDeCarrera(FinalDeCarrera& finXmin,
                                                    FinalDeCarrera& finXmax,
                                                    FinalDeCarrera& finYmin,
                                                    FinalDeCarrera& finYmax) {

    p_finXmin = &finXmin;
    p_finXmax = &finXmax;
    p_finYmin = &finYmin;
    p_finYmax = &finYmax;
}

void PlanificadorDeMovimiento::acelerarTiemposDePaso(int64_t& tiempoPasoX, int64_t& tiempoPasoY) {

    // --- Aceleración / frenado ---
    int pasosRestantes = calcularPasosRestantes(); // vector total
    int pasosTotales = std::max(p_motorX->totalPasos, p_motorY->totalPasos);
    int pasosRecorridos = pasosTotales - pasosRestantes;


    if ((pasosRestantes > pasosParada) && debeAcelerar) {

        // --- Aceleración ---
        velocidadCoef = sqrt(2.0f * aceleracion * pasosRecorridos) / velocidadPasosPorSegundo;
        if (velocidadCoef > 1.0f) velocidadCoef = 1.0f;

    } else if ((pasosRestantes <= pasosParada) && debeFrenar) {

        // --- Deceleración ---
        velocidadCoef = sqrt(2.0f * aceleracion * pasosRestantes) / velocidadPasosPorSegundo;
        if (velocidadCoef > 1.0f) velocidadCoef = 1.0f;
        if (velocidadCoef < 0.1f) velocidadCoef = 0.3f; // evita 0

    } else {
        velocidadCoef = 1.0f;
    }

    // --- Aplica coeficiente a los intervalos ---
    float inversaVelocidadCoef = 1.0f / velocidadCoef;

    int64_t tiempoPasoXacelerado = static_cast<int64_t>(tiempoPasoX * inversaVelocidadCoef);
    int64_t tiempoPasoYacelerado = static_cast<int64_t>(tiempoPasoY * inversaVelocidadCoef);

    if (velocidadCoef > 0.0f) {
        p_motorX->ponTiempoDePaso(tiempoPasoXacelerado);
        p_motorY->ponTiempoDePaso(tiempoPasoYacelerado);
    }
}

void PlanificadorDeMovimiento::moverA(float x, float y, const std::optional<std::pair<float, float>>& siguienteG1) {
    printf("PlanificadorDeMovimiento::moverA x: %f, y: %f \n", x, y);

    if (paradaEmergencia) {
        printf("Por lo visto paradaEmergencia es true :\n");
        return;
    }

    // Calcular los pasos necesarios de cada motor para llegar a la posicion deseada
    int pasosMotorX = 0;
    int pasosMotorY = 0;
    calcularPasos(x, y, pasosMotorX, pasosMotorY);

    // Calcular los tiempos de paso para cada motor en función de la velocidad maxima
    int64_t tiempoPasoX = 10000;
    int64_t tiempoPasoY = 10000;
    calcularTiemposDePaso(abs(pasosMotorX), abs(pasosMotorY), tiempoPasoX, tiempoPasoY);

    // Me guardo los signos de direccion de los motores 
    sentidoMX_actual = (pasosMotorX > 0) - (pasosMotorX < 0); // -1, 0 o 1
    sentidoMY_actual = (pasosMotorY > 0) - (pasosMotorY < 0); // -1, 0 o 1

    debeAcelerar = get_debeAcelerar();
    debeFrenar = get_debeFrenar(x, y, siguienteG1);

    // Configurar los motores con los pasos y tiempos calculados
    configurarMotores(pasosMotorX, pasosMotorY, tiempoPasoX, tiempoPasoY);

    velocidadCoef = 0.0f; // empieza desde 0 (parado)
    coefStep = 0.0f;

    // Guardamos la ultima posición en x_ultimo e y_ultimo
    guardarUltimaPosicion();

    // Activa los motores energizando el pin enable_pin.
    arrancar();

    bool finPorCarrera = false;

    // Iniciamos el bucle donde el motor estará rotando
    while (movimientoEnCurso() && !paradaEmergencia) {
        if (alcanzaFinalDeCarrera()) {
            finPorCarrera = true;
            break;
        }

        // Actualiza el coeficiente de aceleracion en frenada y arranque
        acelerarTiemposDePaso(tiempoPasoX, tiempoPasoY);

        rotar();
        // Se actualiza las coordenadas de posición actual
        calcularPosicionActual(p_motorX->pasoActual, p_motorY->pasoActual, sentidoMX_actual, sentidoMY_actual);
        detenerSiCompletado();
    }

    std::cout << "[PlanificadorDeMovimiento] : x_ultimo:  " << x_ultimo << ", y_ultimo: " << y_ultimo << std::endl;

    // Liberamos la tensión de los motores y así no quedan zumbando
    detener();

    sentidoMX_ultimo = sentidoMX_actual;
    sentidoMY_ultimo = sentidoMY_actual;

    if (finPorCarrera) {
        actualizarPosicionPorPisarFinalDeCarrera();
    } 

    enviarPosicionFifo(); // Enviar la posición actual por FIFO al cliente web
}

void PlanificadorDeMovimiento::moverRelativo(float deltaX, float deltaY,
                                             const std::optional<std::pair<float, float>>& siguienteG1) {
    moverA(x_actual + deltaX, y_actual + deltaY, siguienteG1);
}

void PlanificadorDeMovimiento::moverZ(int z) {
    if (z > 0) p_servoBoli->levantar();
    else p_servoBoli->bajar();
}

void PlanificadorDeMovimiento::calcularPasos(float x, float y,
                                             int& pasosMotorX,
                                             int& pasosMotorY) {
    // Usamos la geometría H-Bot para calcular los pasos necesarios.
    float deltaX = x - x_actual;
    float deltaY = y - y_actual;

    pasosMotorX = static_cast<int>(std::round(Fisicas::resolucionPaso * (-deltaX - deltaY)));
    pasosMotorY = static_cast<int>(std::round(Fisicas::resolucionPaso * (-deltaX + deltaY)));
}

void PlanificadorDeMovimiento::calcularPasos(float x, float y, float posicionSiguienteX, float posicionSiguienteY, int& pasosMotorX, int& pasosMotorY) {

    float deltaX = posicionSiguienteX - x;
    float deltaY = posicionSiguienteY - y;

    pasosMotorX = static_cast<int>(std::round(Fisicas::resolucionPaso * (-deltaX - deltaY)));
    pasosMotorY = static_cast<int>(std::round(Fisicas::resolucionPaso * (-deltaX + deltaY)));
}

void PlanificadorDeMovimiento::calcularPosicionActual(int pasosMotorX, int pasosMotorY,
                                                      int sentidoMX, int sentidoMY) {

    int pasosX_con_s = pasosMotorX * sentidoMX;
    int pasosY_con_s = pasosMotorY * sentidoMY;

    // Usamos la geometría H-Bot para calcular los x segun los pasos.
    float deltaX = -(pasosX_con_s + pasosY_con_s) / (2.0f * Fisicas::resolucionPaso);
    float deltaY = (pasosY_con_s - pasosX_con_s) / (2.0f * Fisicas::resolucionPaso);


    x_actual = x_ultimo + deltaX;
    y_actual = y_ultimo + deltaY;
}


void PlanificadorDeMovimiento::calcularTiemposDePaso(const float absPasosMotorX,
                                                     const float absPasosMotorY,
                                                     int64_t& tiempoPasoX,
                                                     int64_t& tiempoPasoY) {

    if (absPasosMotorX != 0 && absPasosMotorY != 0) {
        float relacionXY = static_cast<float>(absPasosMotorX) / absPasosMotorY;

        if (relacionXY <= 1) {
            tiempoPasoY = static_cast<int64_t>((1.0f / velocidadPasosPorSegundo) * 1000000);
            tiempoPasoX = static_cast<int64_t>(static_cast<float>(tiempoPasoY) / relacionXY);
        } else {
            tiempoPasoX = static_cast<int64_t>((1.0f / velocidadPasosPorSegundo) * 1000000);
            tiempoPasoY = static_cast<int64_t>(static_cast<float>(tiempoPasoX) * relacionXY);
        }
    } else if (absPasosMotorX == 0) {
        tiempoPasoY = static_cast<int64_t>((1.0f / velocidadPasosPorSegundo) * 1000000);
    } else if (absPasosMotorY == 0) {
        tiempoPasoX = static_cast<int64_t>((1.0f / velocidadPasosPorSegundo) * 1000000);
    }
}

int PlanificadorDeMovimiento::calcularPasosRestantes() {
    int pasosX = p_motorX->totalPasos - p_motorX->pasoActual;
    int pasosY = p_motorY->totalPasos - p_motorY->pasoActual;
    return std::max(pasosX, pasosY); // para rampas escaladas
}

void PlanificadorDeMovimiento::guardarUltimaPosicion() {
    x_ultimo = x_actual;
    y_ultimo = y_actual;
}

void PlanificadorDeMovimiento::rotar() {
    p_motorX->rotar();
    p_motorY->rotar();
}

void PlanificadorDeMovimiento::detenerSiCompletado() {
    p_motorX->detenerSiCompletado();
    p_motorY->detenerSiCompletado();
}

void PlanificadorDeMovimiento::detener() {
    p_motorX->parar();
    p_motorY->parar();
}

void PlanificadorDeMovimiento::arrancar() {
    p_motorX->arrancar();
    p_motorY->arrancar();
}

bool PlanificadorDeMovimiento::movimientoEnCurso() {
    return (p_motorX->estaRotando || p_motorY->estaRotando);
}

bool PlanificadorDeMovimiento::alcanzaFinalDeCarrera() {

    return comprobarFin(p_finXmin, "Xmin") ||
        comprobarFin(p_finXmax, "Xmax") ||
        comprobarFin(p_finYmin, "Ymin") ||
        comprobarFin(p_finYmax, "Ymax");
}

void PlanificadorDeMovimiento::actualizarPosicionPorPisarFinalDeCarrera() {
    // TODO: Si ha tocado un limite debemos actualizar el valor de dicho eje con el correspondiente al limite

    if (ultimoFinDeCarreraActivado == "Xmin") {
        x_actual = -parametros.getAncho() / 2;
        std::cout << "[PlanificadorDeMovimiento] : "
            << "Ancho es:  " << parametros.getAncho()
            << ", x_actual: " << x_actual << std::endl;

    } else if (ultimoFinDeCarreraActivado == "Xmax") {
        x_actual = parametros.getAncho() / 2;

    } else if (ultimoFinDeCarreraActivado == "Ymin") {
        y_actual = -parametros.getAlto() / 2;

    } else if (ultimoFinDeCarreraActivado == "Ymax") {
        y_actual = parametros.getAlto() / 2;
    }
}

bool PlanificadorDeMovimiento::comprobarFin(FinalDeCarrera* sensor, const std::string& nombre) {
    if (sensor->esPulsado()) {
        ultimoFinDeCarreraActivado = nombre;

        // TODO: Faltará meter el autohome y diferenciar este caso para la parada de emergencia
        activarParadaDeEmergencia();

        std::cout << "[PlanificadorDeMovimiento] Final de carrera: " << nombre << " activado. Parada de emergencia." << std::endl;
        FifoWriter::write("[Parada] Final de carrera: " + nombre);

        return true;
    }
    return false;
}


std::pair<float, float> PlanificadorDeMovimiento::obtenerPosicion() {
    return { x_actual, y_actual };
}

void PlanificadorDeMovimiento::actualizarPosicion(float x, float y) {
    x_actual = x;
    y_actual = y;
}

void PlanificadorDeMovimiento::enviarPosicionFifo() {
    if (FifoWriter::isReady()) {
        FifoWriter::write("Posicion actual: (" + std::to_string(x_actual) + ", " + std::to_string(y_actual) + ")");
    } else {
        std::cerr << "[PlanificadorDeMovimiento] No se pudo enviar la posición por FIFO" << std::endl;
    }
}

void PlanificadorDeMovimiento::configurarMotores(int pasosMotorX, int pasosMotorY, int64_t tiempoPasoX, int64_t tiempoPasoY) {
    p_motorX->rotarPasos(pasosMotorX);
    p_motorY->rotarPasos(pasosMotorY);
    p_motorX->ponTiempoDePaso(tiempoPasoX);
    p_motorY->ponTiempoDePaso(tiempoPasoY);

}

bool PlanificadorDeMovimiento::get_debeAcelerar() {
    if (sentidoMX_actual != sentidoMX_ultimo || sentidoMY_actual != sentidoMY_ultimo) {
        //std::cout << "[PlanificadorDeMovimiento] debeAcelerar: true" << std::endl;
        return true;
    }
    //std::cout << "[PlanificadorDeMovimiento] debeAcelerar: false" << std::endl;
    return false;
}

bool PlanificadorDeMovimiento::get_debeFrenar(float x, float y,
        const std::optional<std::pair<float, float>>& siguienteG1) {
    
    // Si no hay siguiente punto, debe frenar
    if (!siguienteG1.has_value()) {
        return true;
    }

     // --- Calcular los pasos del siguiente punto ---
    int pasosMotorSiguienteX = 0;
    int pasosMotorSiguienteY = 0;

    calcularPasos(x, y,
        siguienteG1->first,
        siguienteG1->second,
        pasosMotorSiguienteX,
        pasosMotorSiguienteY);

    //  --- Determinar el sentido de los motores ---
    sentidoMX_siguiente = (pasosMotorSiguienteX > 0) - (pasosMotorSiguienteX < 0); // -1, 0 o 1
    sentidoMY_siguiente = (pasosMotorSiguienteY > 0) - (pasosMotorSiguienteY < 0); //  

    // --- Comprobar si cambia el sentido de algún motor ---
    bool cambiaSentidoX = (sentidoMX_actual != sentidoMX_siguiente);
    bool cambiaSentidoY = (sentidoMY_actual != sentidoMY_siguiente);

    bool debeFrenar = (cambiaSentidoX || cambiaSentidoY);

    return debeFrenar;    
}

void PlanificadorDeMovimiento::activarParadaDeEmergencia() {
    paradaEmergencia = true;
    p_servoBoli->liberar();

    FifoWriter::write("[Parada] Emergencia: true");
    detener(); // Detener todos los motores inmediatamente
}

void PlanificadorDeMovimiento::desactivarParadaDeEmergencia() {
    paradaEmergencia = false;
    p_servoBoli->liberar();

    FifoWriter::write("[Parada] Emergencia: false");
}

bool PlanificadorDeMovimiento::esParadaDeEmergencia() const {
    return paradaEmergencia;
}

void PlanificadorDeMovimiento::enviarInformacionGeneralFifo() {
    if (FifoWriter::isReady()) {
        std::string info = "Informacion General: ";
        info += "pos_actual: (" + std::to_string(x_actual) + ", " + std::to_string(y_actual) + "); ";
        info += "vel_unit_max: " + std::to_string(velocidadUnitariaMax) + "; ";
        info += "vel_ang_max: " + std::to_string(velocidadAngularMax) + "; ";
        info += "parada_emergencia: " + std::string(paradaEmergencia ? "true" : "false") + "; ";
        info += "u_final_carrera: " + ultimoFinDeCarreraActivado;

        FifoWriter::write(info);
    } else {
        std::cerr << "[PlanificadorDeMovimiento] No se pudo enviar la información general por FIFO" << std::endl;
    }
}
