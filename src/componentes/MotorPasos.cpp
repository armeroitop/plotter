#include "MotorPasos.hpp"

MotorPasos::MotorPasos(int pinA, int pinB, int pinC, int pinD)
    : pinA{pinA}, pinB{pinB}, pinC{pinC}, pinD{pinD}, fisica() {
  wiringPiSetup();
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
}

void MotorPasos::energizarBobinaActual() {
  // Secuencia tipo wave drive
  digitalWrite(pinD,
               (secuenciaPaso == 1) ? HIGH : LOW);  // secuenciaPaso 1 (0,0,0,1)
  digitalWrite(pinB,
               (secuenciaPaso == 2) ? HIGH : LOW);  // secuenciaPaso 2 (0,1,0,0)
  digitalWrite(pinC,
               (secuenciaPaso == 3) ? HIGH : LOW);  // secuenciaPaso 3 (0,0,1,0)
  digitalWrite(pinA,
               (secuenciaPaso == 4) ? HIGH : LOW);  // secuenciaPaso 4 (1,0,0,0)

  // Secuencia tipo normal

  //digitalWrite(pinD,
  //             (secuenciaPaso == 2 || secuenciaPaso == 3) ? HIGH : LOW);  // secuenciaPaso 1 (1,0,1,0)
  //digitalWrite(pinB,
  //             (secuenciaPaso == 3 || secuenciaPaso == 4) ? HIGH : LOW);  // secuenciaPaso 2 (1,0,0,1)
  //digitalWrite(pinC,
  //             (secuenciaPaso == 1 || secuenciaPaso == 4) ? HIGH : LOW);  // secuenciaPaso 3 (0,1,0,1)
  //digitalWrite(pinA,
  //             (secuenciaPaso == 1 || secuenciaPaso == 2) ? HIGH : LOW);  // secuenciaPaso 4 (0,1,1,0)
}

void MotorPasos::siguientePaso() {
  if (sentidoGiro == SentidoGiro::Horario) {
    // Aumentar paso en sentido horario
    if (secuenciaPaso + 1 > 4) {
      secuenciaPaso = 1;
    } else {
      ++secuenciaPaso;
    }
  } else if (sentidoGiro == SentidoGiro::Antihorario) {
    // Disminuir paso en sentido antihorario:
    if (secuenciaPaso - 1 < 1) {
      secuenciaPaso = 4;
    } else {
      --secuenciaPaso;
    }
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
    std::cout << "El motor está ocupado completando la tarea anterior."
              << std::endl;
    return;
  }

  // Iniciar una nueva tarea
  sentidoGiro = (std::copysign(1, cantidadPasos) > 0)
                    ? SentidoGiro::Horario
                    : SentidoGiro::Antihorario;
  pasosTotal = abs(cantidadPasos);
  pasoActual = 0;
  estaRotando = true;
}

void MotorPasos::rotar() {
  // si ha pasado el tiempo necesario entre casa paso y si no ha completado ya
  // todos los pasos, podrá dar el siguiente paso
  if (haPasadoTiempoDePaso() && !haCompletadoPasos()) {
    ponTiempoDeInicio();
    siguientePaso();
    energizarBobinaActual();
    pasoActual++;

    printf("En rotar mi paso es%d \n", pasoActual);
    printf("En rotar tiempodeInicio es %lld  y tiempo es %lld\n",
           tiempoDeInicio, obtenerTiempoActualMs());
  }

  if (haCompletadoPasos()) {
    // cuando de el ultimo paso cambiará "estaRotando" a false
    pasoActual = 0;
    estaRotando = false;
  }
}

void MotorPasos::estableceSentidoGiro(int pasos) {
  int signo = (std::copysign(1.0, pasos));

  if (signo < 0) {
    sentidoGiro = SentidoGiro::Antihorario;
  } else if (signo > 0) {
    sentidoGiro = SentidoGiro::Horario;
  } else {
    sentidoGiro = SentidoGiro::Parado;
  }
}

bool MotorPasos::haCompletadoPasos() { return (pasoActual >= pasosTotal); }
