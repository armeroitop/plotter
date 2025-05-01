#pragma once
#include <wiringPi.h>
#include <softPwm.h>
#include <iostream>

struct ServoBoli {
    int pin;         // Pin de control del servo
    int anguloLevantado;     // Ángulo cuando el bolígrafo está levantado
    int anguloBajado;   // Ángulo cuando el bolígrafo está abajo
    bool esAbajo;     // Estado actual del servo

    ServoBoli(int pin, int anguloLevantado = 120, int anguloBajado = 0);
    void levantar();
    void bajar();
    bool estaAbajo() const;
    
    void moverServo(int angulo); // Método interno para mover el servo
};
