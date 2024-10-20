#pragma once
#include <wiringPi.h>

struct MotorPasos {
    //https://github.com/LaurensHuizer/Arduino/blob/master/libraries/StepperMotor/StepperMotor.h

    const int pinA, pinB, pinC, pinD; // Definicion de pines
    int _mode = 0;
    int _steps = -1;
    int _delayMs = 1;
    int _engineStep = 0;
    int _currentStep = 0;
    int _rotationDirection = 0;

    MotorPasos(int pinA, int pinB, int pinC, int pinD);

};
