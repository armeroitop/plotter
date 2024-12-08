#pragma once

#include <iostream>
#include "MotorDriver.hpp"


struct L298NDriver : MotorDriver {

  const int pinA, pinB, pinC, pinD;  // Definicion de pines

  L298NDriver(int pinA, int pinB, int pinC, int pinD);

  void siguientePaso() override;

  void rotarPasos(int cantidadPasos) override;

  void parar() override;

  void rotar() override;

  bool haCompletadoPasos() override;

  void energizarBobinaActual() override;

  void estableceSentidoGiro(int pasos) override; 
};
