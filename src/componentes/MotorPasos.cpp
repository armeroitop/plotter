#include "MotorPasos.hpp"


MotorPasos::MotorPasos(int pinA, int pinB, int pinC, int pinD) :
    pinA { pinA }, pinB { pinB }, pinC { pinC }, pinD { pinD } {

    wiringPiSetup();
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinD, OUTPUT);

}

void MotorPasos::energizarBobinaActual() {
    // Configura cada pin según el paso actual
    digitalWrite(pinD, (secuenciaPaso == 1) ? HIGH : LOW);// secuenciaPaso 1   (0,0,0,1)
    digitalWrite(pinB, (secuenciaPaso == 2) ? HIGH : LOW);// secuenciaPaso 2   (0,1,0,0)
    digitalWrite(pinC, (secuenciaPaso == 3) ? HIGH : LOW);// secuenciaPaso 3   (0,0,1,0)
    digitalWrite(pinA, (secuenciaPaso == 4) ? HIGH : LOW);// secuenciaPaso 4   (1,0,0,0)
}

void MotorPasos::siguientePaso() {
    if (sentidoGiro == SentidoGiro::Horario) {
        secuenciaPaso = (secuenciaPaso + 1 > 4) ? 1 : ++secuenciaPaso;
    }
    else if (sentidoGiro == SentidoGiro::Antihorario) {
        secuenciaPaso = (secuenciaPaso - 1 < 1) ? 4 : --secuenciaPaso;
    }
}

void MotorPasos::parar() {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, LOW);
    sentidoGiro = SentidoGiro::Parado;
}

void MotorPasos::rotarPasos(int cantidadPasos) {
    // Si el motor está en funcionamiento, no iniciar una nueva tarea
    if (estaRotando) {
        std::cout << "El motor está ocupado completando la tarea anterior." << std::endl;
        return;
    }

    // Iniciar una nueva tarea
    sentidoGiro = (std::copysign(1, cantidadPasos) > 0) ? SentidoGiro::Horario : SentidoGiro::Antihorario;
    pasosTotal = abs(cantidadPasos);
    pasoActual = 0;
    estaRotando = true;
}

void MotorPasos::estableceSentidoGiro(int pasos) {

    int signo = (std::copysign(1.0, pasos));

    if (signo < 0) {
        sentidoGiro = SentidoGiro::Antihorario;
    }
    else if (signo > 0) {
        sentidoGiro = SentidoGiro::Horario;
    }
    else {
        sentidoGiro = SentidoGiro::Parado;
    }

}
