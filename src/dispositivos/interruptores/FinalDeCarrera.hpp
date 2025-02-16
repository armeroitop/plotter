#pragma once
#include <wiringPi.h>


struct FinalDeCarrera {
    const int pin;

    explicit FinalDeCarrera(int pin);

    bool esPulsado();
};
