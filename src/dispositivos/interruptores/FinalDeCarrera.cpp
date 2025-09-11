#include "FinalDeCarrera.hpp"

FinalDeCarrera::FinalDeCarrera(int pin) : pin { pin } {
    pinMode(pin, INPUT);// INPUT_PULLUP o INPUT_PULLDOWN
    ultimoTiempoRebote = 0;
    retardoRebote = 50; // tiempo de retardo en milisegundos
    ultimoEstadoBoton = LOW;
    estadoBoton = LOW;

}

/**
 * @brief Detecta si el final de carrera ha sido pulsado, con gestión de rebotes.
 * 
 * @return true  si el final de carrera ha sido pulsado
 * @return false si no ha sido pulsado
 */
bool FinalDeCarrera::esPulsado() {
    int lectura = digitalRead(pin);

    if (lectura != ultimoEstadoBoton) {
        ultimoTiempoRebote = millis(); // reinicia el temporizador de rebote
    }

    if ((millis() - ultimoTiempoRebote) > retardoRebote) {
        if (lectura != estadoBoton) {
            estadoBoton = lectura;
            if (estadoBoton == HIGH) {
                return true;
            }
        }
    }

    ultimoEstadoBoton = lectura;
    return false;
}
