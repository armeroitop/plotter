#include "PlanificadorDeMovimiento.hpp"

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
    if (paradaEmergencia) {
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

    printf("tiempoPasoX %i \n", tiempoPasoX);
    printf("tiempoPasoY %i \n", tiempoPasoY);

    // Configurar los motores con los pasos y tiempos calculados
    configurarMotores(pasosMotorX, pasosMotorY, tiempoPasoX, tiempoPasoY);


    // Mover los motores hasta llegar a la posicion. 
    arrancar();

    while (movimientoEnCurso() && !alcanzaFinalDeCarrera()) {
        rotar();
    }

    detener();

    actualizarPosicion(x, y);
}

void PlanificadorDeMovimiento::calcularPasos(float x, float y,
                                             int& pasosMotorX,
                                             int& pasosMotorY) {
    // Usamos la geometría H-Bot para calcular los pasos necesarios.
    float deltaX = x - x_actual;
    float deltaY = y - y_actual;

    pasosMotorX = static_cast<int>(Fisicas::resolucionPaso * (deltaX + deltaY));
    pasosMotorY = static_cast<int>(Fisicas::resolucionPaso * (deltaY - deltaX));
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
        }
        else {
            tiempoPasoX = static_cast<int>(1 / velocidadAngularMax);
            tiempoPasoY = static_cast<int>(tiempoPasoX * relacionXY);
        }
    }
    else if (absPasosMotorX == 0) {
        tiempoPasoY = static_cast<int>(1 / velocidadAngularMax);
    }
    else if (absPasosMotorY == 0) {
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

void PlanificadorDeMovimiento::obtenerPosicion(float& x, float& y) { }

void PlanificadorDeMovimiento::actualizarPosicion(float x, float y) {
    x_actual = x;
    y_actual = y;
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