#include "DRV8825Driver.hpp"

#include <cmath>

DRV8825Driver::DRV8825Driver(int step_pin, int dir_pin)
    : MotorDriver(), step_pin{step_pin}, dir_pin{dir_pin} {
  wiringPiSetup();
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
}

void DRV8825Driver::siguientePaso() {}

void DRV8825Driver::rotarPasos(int cantidadPasos) {
  printf("inicio de rotar\n");
  estableceSentidoGiro(cantidadPasos);
  
  for (int i = 0; i < std::abs(cantidadPasos) ; i++) {
    digitalWrite(step_pin, LOW);
    delayMicroseconds(2000);
    digitalWrite(step_pin, HIGH);
    delayMicroseconds(2000);
  }
  printf("fin de rotar\n");
}

void DRV8825Driver::parar() {}

void DRV8825Driver::rotar() {}

bool DRV8825Driver::haCompletadoPasos() { return false; }

void DRV8825Driver::energizarBobinaActual() {}

void DRV8825Driver::estableceSentidoGiro(int pasos) {
  
  if (pasos < 0) {
    digitalWrite(dir_pin, LOW);
  } else {
    digitalWrite(dir_pin, HIGH);
  }
}
