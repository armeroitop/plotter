#pragma once

#include "MotorDriver.hpp"

struct DRV8825Driver : MotorDriver {

  const int step_pin, dir_pin;  // Definicion de pines

  DRV8825Driver(int step_pin, int dir_pin);

  void siguientePaso() override;

  void rotarPasos(int cantidadPasos) override;

  void parar() override;

  void rotar() override;

  bool haCompletadoPasos() override;

  void energizarBobinaActual() override;

  void estableceSentidoGiro(int pasos) override;
};
