#include "PlanificadorDeMovimiento.hpp"

PlanificadorDeMovimiento::PlanificadorDeMovimiento() { }

void PlanificadorDeMovimiento::setMotores(MotorDriver& motorX,
                                          MotorDriver& motorY) {
    p_motorX = &motorX;
    p_motorY = &motorY;
}

void PlanificadorDeMovimiento::moverA(float x, float y) {
    // TODO ver que posicion xy tenemos

    // TODO calcular los pasos necesarios de cada motor para llegar a la posicion
    // deseada
    float deltaX = x - x_actual;
    float deltaY = y - y_actual;
    int pasosMotorX = 0;
    int pasosMotorY = 0;
    calcularPasos(deltaX, deltaY, pasosMotorX, pasosMotorY);

    // TODO calcular los tiempos de paso para cada motor en función de la
    // velocidad maxima
    int tiempoPasoX = 10000;
    int tiempoPasoY = 10000;

    int absPasosMotorX = abs(pasosMotorX);
    int absPasosMotorY = abs(pasosMotorY);

    // TODO meter esto en una funcion
    if (absPasosMotorX != 0 && absPasosMotorY != 0) {
        float relacionXY = static_cast<float>(absPasosMotorX) / absPasosMotorY;

        if (relacionXY <= 1) {
            tiempoPasoY = static_cast<int>(1 / velocidadAngularMax);
            tiempoPasoX = tiempoPasoX * relacionXY;
        } else {
            tiempoPasoX = static_cast<int>(1 / velocidadAngularMax);
            tiempoPasoY = tiempoPasoX * relacionXY;
        }
    } else if (absPasosMotorX == 0) {
        tiempoPasoY = static_cast<int>(1 / velocidadAngularMax);
    } else if (absPasosMotorY == 0) {
        tiempoPasoX = static_cast<int>(1 / velocidadAngularMax);
    }

    printf("tiempoPasoX %i \n",tiempoPasoX);
    printf("tiempoPasoY %i \n",tiempoPasoY);

    // TODO mover los motores hasta llegar a la posicion. Meter un while
    // verificando si hay movimiento en curso
    p_motorX->rotarPasos(pasosMotorX);
    p_motorY->rotarPasos(pasosMotorY);
    p_motorX->ponTiempoDePaso(tiempoPasoX);
    p_motorY->ponTiempoDePaso(tiempoPasoY);

    while (p_motorX->estaRotando or p_motorY->estaRotando) {
        p_motorX->rotar();
        p_motorY->rotar();
        // printf("Paso actual %d \n",p_motorX->pasoActual);
    }
    p_motorX->parar();
    p_motorY->parar();
}

void PlanificadorDeMovimiento::calcularPasos(float deltaX, float deltaY,
                                             int& pasosMotorX,
                                             int& pasosMotorY) {
    // Usamos la geometría H-Bot para calcular los pasos necesarios.

    pasosMotorX = static_cast<int>(Fisicas::resolucionPaso * (deltaX + deltaY));
    pasosMotorY = static_cast<int>(Fisicas::resolucionPaso * (deltaY - deltaX));
}

void PlanificadorDeMovimiento::obtenerPosicion(float& x, float& y) { }