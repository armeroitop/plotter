#pragma once

#include "MotorDriver.hpp"

struct DRV8825Driver : MotorDriver {

  const int step_pin, dir_pin, enable_pin;  // Definicion de pines

  DRV8825Driver(int step_pin, int dir_pin, int enable_pin);

  void siguienteSecuencia() override;

  void rotarPasos(int cantidadPasos) override;

  void parar() override;

  void arrancar() override;

  void rotar() override;

  bool haCompletadoPasos() override;

  /**
   * @brief Aplica tensión al pin 'step_pin' según la secuencia de paso del motor.
   * 
   * Por cada paso se debe alternar entre HIGH y LOW.
   */
  void energizarBobinaActual() override;

  void estableceSentidoGiro(int pasos) override;
};
