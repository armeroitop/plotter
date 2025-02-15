#include "FinalDeCarrera.hpp"

FinalDeCarrera::FinalDeCarrera(int pin) : pin { pin } {
    pinMode(pin, INPUT);// INPUT_PULLUP o INPUT_PULLDOWN

}

bool FinalDeCarrera::esPulsado() {
    //return digitalRead(pin) == LOW;
    return false;
}
