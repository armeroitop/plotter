#pragma once
#include <wiringPi.h>


struct FinalDeCarrera {
    const int pin;
    unsigned long ultimoTiempoRebote;
    unsigned long retardoRebote;
    int ultimoEstadoBoton;
    int estadoBoton;

    explicit FinalDeCarrera(int pin);

    bool esPulsado();
};
