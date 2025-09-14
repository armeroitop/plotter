#pragma once

#include <iostream>
#include "MotorDriver.hpp"


struct L298NDriver : MotorDriver {

    const int pinA, pinB, pinC, pinD;  // Definicion de pines

    L298NDriver(int pinA, int pinB, int pinC, int pinD);

    ~L298NDriver() override;
    
    void siguienteSecuencia() override;

    void rotarPasos(int cantidadPasos) override;

    void parar() override;

    void rotar() override;

    void reset() override;

    bool haCompletadoPasos() override;

    /**
     * @brief Aplica tensión al pin correspondiente según el paso actual del
     * motor.
     *
     * Este método activa el pin adecuado en función del valor de `secuenciaPaso`,
     * estableciendo el pin en alto (HIGH) para energizar la bobina
     * correspondiente al paso. Cada paso sigue una secuencia que permite el giro
     * controlado del motor.
     *
     * - Paso 1: Activa `pinD`
     * - Paso 2: Activa `pinB`
     * - Paso 3: Activa `pinC`
     * - Paso 4: Activa `pinA`
     */
    void energizarBobinaActual() override;

    void estableceSentidoGiro(int pasos) override;
};
