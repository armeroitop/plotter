#include "PlanificadorDeMovimiento.hpp"
#include "../dispositivos/interruptores/FinalDeCarrera.hpp"
#include "../dispositivos/motores/DRV8825Driver.hpp"
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


    // TODO Falta implementar si es necesario ac/dcelerar solo si estamos cambiando de direccion

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
    int64_t tiempoPasoXacelerado = static_cast<int64_t>(tiempoPasoX / velocidadCoef);
    int64_t tiempoPasoYacelerado = static_cast<int64_t>(tiempoPasoY / velocidadCoef);

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

    // Me guardo los signos de direccion de los motores (TODO sería bueno usar p_motorX->sentidoGiro y quitar esto de aquí)
    sentidoMX_actual = (pasosMotorX > 0) - (pasosMotorX < 0); // -1, 0 o 1
    sentidoMY_actual = (pasosMotorY > 0) - (pasosMotorY < 0); // -1, 0 o 1


    debeFrenar = true; // debe frenar siempre a no ser que realmente no sea necesario
    if (siguienteG1.has_value()) {

        printf("siguienteG1: %f, %f \n", siguienteG1->first, siguienteG1->second);
        // TODO Habría que consultar si para el proximo movimiento hay un cambio de sentido en los motores
        // 1. calcular la posicion final del movimiento actual -> será x e y
        // 2. ver los pasos desde la posicion algual a la siguienteG1
        int pasosMotorSiguienteX = 0;
        int pasosMotorSiguienteY = 0;
        calcularPasos(x, y,
            siguienteG1->first,
            siguienteG1->second,
            pasosMotorSiguienteX,
            pasosMotorSiguienteY);

        // 3. ver el sentido de los pasos
        sentidoMX_siguiente = (pasosMotorSiguienteX > 0) - (pasosMotorSiguienteX < 0); // -1, 0 o 1
        sentidoMY_siguiente = (pasosMotorSiguienteY > 0) - (pasosMotorSiguienteY < 0); //  

        // 4. ver si hay diferencia con los que está dando para llegar al movimiento acutal
        if (sentidoMX_actual != sentidoMX_siguiente || sentidoMY_actual != sentidoMY_siguiente) {
            debeFrenar = true;
        } else {
            debeFrenar = false;
        }
    }

    debeAcelerar = get_debeAcelerar();

    // Configurar los motores con los pasos y tiempos calculados
    configurarMotores(pasosMotorX, pasosMotorY, tiempoPasoX, tiempoPasoY);

    velocidadCoef = 0.0f; // empieza desde 0 (parado)
    coefStep = 0.0f;

    // Guardamos la ultima posición en x_ultimo e y_ultimo
    guardarUltimaPosicion();

    // Mover los motores hasta llegar a la posicion.
    arrancar();

    bool finPorCarrera = false;

    // Iniciamos el bucle donde el motor estará rotando
    while (movimientoEnCurso() && !paradaEmergencia) {
        if (alcanzaFinalDeCarrera()) {
            finPorCarrera = true;
            break;
        }

        // TODO aquí se ha de actualizar el coeficiente de aceleracion
        acelerarTiemposDePaso(tiempoPasoX, tiempoPasoY);

        rotar();
        // Se actualiza las coordenadas de posición actual
        calcularPosicionActual(p_motorX->pasoActual, p_motorY->pasoActual, sentidoMX_actual, sentidoMY_actual);
        detenerSiCompletado();
    }

    // FIXME: Este metodo no me esta funcionando
    //calcularPosicionActual(p_motorX->pasoActual, p_motorY->pasoActual);
    std::cout << "[PlanificadorDeMovimiento] : x_ultimo:  " << x_ultimo << ", y_ultimo: " << y_ultimo << std::endl;

    detener();

    sentidoMX_ultimo = sentidoMX_actual;
    sentidoMY_ultimo = sentidoMY_actual;

    if (finPorCarrera) {
        actualizarPosicionPorPisarFinalDeCarrera();
    } else {
        // ya venimos con las coordenadas actualizadas
        //actualizarPosicion(x, y);
    }
    enviarPosicionFifo(); // Enviar la posición actual por FIFO al cliente web
}

void PlanificadorDeMovimiento::moverRelativo(float deltaX, float deltaY,
                                             const std::optional<std::pair<float, float>>& siguienteG1) {
    moverA(x_actual + deltaX, y_actual + deltaY, siguienteG1);
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
    return false;
}

void PlanificadorDeMovimiento::activarParadaDeEmergencia() {
    paradaEmergencia = true;

    FifoWriter::write("[Parada] Emergencia: true");
    detener(); // Detener todos los motores inmediatamente
}

void PlanificadorDeMovimiento::desactivarParadaDeEmergencia() {
    paradaEmergencia = false;

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
