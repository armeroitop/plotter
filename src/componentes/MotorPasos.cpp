#include "MotorPasos.hpp"


MotorPasos::MotorPasos(int pinA, int pinB, int pinC, int pinD) :
    pinA { pinA }, pinB { pinB }, pinC { pinC }, pinD { pinD } {

    wiringPiSetup();
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinD, OUTPUT);

}