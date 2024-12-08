#pragma once

#include "MotorDriver.hpp"

struct DRV8825Driver : MotorDriver {
  const int pinA, pinB, pinC, pinD;  // Definicion de pines

  DRV8825Driver(int pinA, int pinB, int pinC, int pinD);

  void siguientePaso() override;

  void rotarPasos(int cantidadPasos) override;

  void parar() override;

  void rotar() override;

  bool haCompletadoPasos() override;

  void energizarBobinaActual() override;

  void estableceSentidoGiro(int pasos) override;
};
