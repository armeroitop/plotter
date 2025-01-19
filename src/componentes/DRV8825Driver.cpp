#include "DRV8825Driver.hpp"

#include <cmath>

DRV8825Driver::DRV8825Driver(int step_pin, int dir_pin, int enable_pin)
    : MotorDriver(),
      step_pin{step_pin},
      dir_pin{dir_pin},
      enable_pin{enable_pin} {
  wiringPiSetup();
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, LOW);
  ponTiempoDePaso(100);
  secuenciaPaso = 1;
}

void DRV8825Driver::siguientePaso() {
  // La secuencia de paso sera de 1 a 2, o en alto o en bajo
  secuenciaPaso = (secuenciaPaso == 1) ? 2 : 1;
  pasoActual++;
}

void DRV8825Driver::rotarPasos(int cantidadPasos) {
  // Si el motor está en funcionamiento, no iniciar una nueva tarea
  if (estaRotando) {
    printf("Rotar ocupado \n");
    return;
  }

  // Iniciar una nueva tarea
  printf("inicio de rotar\n");
  estableceSentidoGiro(cantidadPasos);
  pasosTotal = abs(cantidadPasos);
  pasoActual = 0;
  estaRotando = true;
}

void DRV8825Driver::rotar() {
  // si ha pasado el tiempo necesario entre casa paso y si no ha completado ya
  // todos los pasos, podrá dar el siguiente paso
  if (haPasadoTiempoDePaso() && !haCompletadoPasos()) {
    ponTiempoDeInicio();
    siguientePaso();
    energizarBobinaActual();

    printf("En rotar mi paso es %d \n", pasoActual);
    printf("En rotar tiempodeInicio es %lld  y tiempo es %lld\n",
           tiempoDeInicio, obtenerTiempoActualMs());
  }

  if (haCompletadoPasos()) {
    // cuando dé el último paso cambiará "estaRotando" a false
    pasoActual = 0;
    estaRotando = false;
  }
}

bool DRV8825Driver::haCompletadoPasos() { return (pasoActual >= pasosTotal); }

void DRV8825Driver::energizarBobinaActual() {
  digitalWrite(step_pin, (secuenciaPaso == 1) ? HIGH : LOW);
}

void DRV8825Driver::estableceSentidoGiro(int pasos) {
  if (pasos < 0) {
    sentidoGiro = SentidoGiro::Antihorario;
    digitalWrite(dir_pin, LOW);
  } else {
    sentidoGiro = SentidoGiro::Horario;
    digitalWrite(dir_pin, HIGH);
  }
}

void DRV8825Driver::parar() { digitalWrite(enable_pin, HIGH); }
