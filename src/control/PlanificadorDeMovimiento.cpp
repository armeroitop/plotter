#include "PlanificadorDeMovimiento.hpp"
#include "fifo/FifoWriter.hpp"
#include <cmath>

PlanificadorDeMovimiento::PlanificadorDeMovimiento() { }

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

void PlanificadorDeMovimiento::moverA(float x, float y) {
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
    int tiempoPasoX = 10000;
    int tiempoPasoY = 10000;
    calcularTiemposDePaso(abs(pasosMotorX), abs(pasosMotorY), tiempoPasoX, tiempoPasoY);

    //printf("tiempoPasoX %i \n", tiempoPasoX);
    //printf("tiempoPasoY %i \n", tiempoPasoY);

    // Configurar los motores con los pasos y tiempos calculados
    configurarMotores(pasosMotorX, pasosMotorY, tiempoPasoX, tiempoPasoY);


    // Mover los motores hasta llegar a la posicion. 
    arrancar();

    while (movimientoEnCurso() && !alcanzaFinalDeCarrera() && !paradaEmergencia) {
        rotar();
        // TODO: Se debería actualizar la posición actual aquí
    }

    detener();

    actualizarPosicion(x, y);
    enviarPosicionFifo(); // Enviar la posición actual por FIFO al cliente web
}

void PlanificadorDeMovimiento::moverRelativo(float deltaX, float deltaY) {
    moverA(x_actual + deltaX, y_actual + deltaY);
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

void PlanificadorDeMovimiento::calcularTiemposDePaso(const float absPasosMotorX,
                                                     const float absPasosMotorY,
                                                     int& tiempoPasoX,
                                                     int& tiempoPasoY) {

    if (absPasosMotorX != 0 && absPasosMotorY != 0) {
        float relacionXY = static_cast<float>(absPasosMotorX) / absPasosMotorY;

        if (relacionXY <= 1) {
            tiempoPasoY = static_cast<int>(1 / velocidadAngularMax);
            tiempoPasoX = static_cast<int>(tiempoPasoY / relacionXY);
        } else {
            tiempoPasoX = static_cast<int>(1 / velocidadAngularMax);
            tiempoPasoY = static_cast<int>(tiempoPasoX * relacionXY);
        }
    } else if (absPasosMotorX == 0) {
        tiempoPasoY = static_cast<int>(1 / velocidadAngularMax);
    } else if (absPasosMotorY == 0) {
        tiempoPasoX = static_cast<int>(1 / velocidadAngularMax);
    }
}

void PlanificadorDeMovimiento::rotar() {
    p_motorX->rotar();
    p_motorY->rotar();
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
    if (p_finXmin->esPulsado()
            || p_finXmax->esPulsado()
            || p_finYmin->esPulsado()
            || p_finYmax->esPulsado()) {
        activarParadaDeEmergencia();
        return true;
    }

    return false;
}

void PlanificadorDeMovimiento::obtenerPosicion(float& x, float& y) { 

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

void PlanificadorDeMovimiento::configurarMotores(int pasosMotorX, int pasosMotorY, int tiempoPasoX, int tiempoPasoY) {
    p_motorX->rotarPasos(pasosMotorX);
    p_motorY->rotarPasos(pasosMotorY);
    p_motorX->ponTiempoDePaso(tiempoPasoX);
    p_motorY->ponTiempoDePaso(tiempoPasoY);
    
}

void PlanificadorDeMovimiento::activarParadaDeEmergencia() {
    paradaEmergencia = true;
    detener(); // Detener todos los motores inmediatamente
}

void PlanificadorDeMovimiento::desactivarParadaDeEmergencia() {
    paradaEmergencia = false;
}

bool PlanificadorDeMovimiento::esParadaDeEmergencia() const {
    return paradaEmergencia;
}