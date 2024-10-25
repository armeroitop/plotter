#pragma once
#include <iostream>
#include <wiringPi.h>
#include <cmath>
#include "Tiempo.hpp"

/// @brief 
struct MotorPasos : public Tiempo {
  //https://github.com/LaurensHuizer/Arduino/blob/master/libraries/StepperMotor/StepperMotor.h

  const int pinA, pinB, pinC, pinD; // Definicion de pines

  /*Número de pasos por una vuelta*/
  const int npasos = 0;
  /*Paso en el que se encuentra actualmente*/
  int secuenciaPaso = 0;
  /*Número de pasos a dar por instrucción*/
  int pasosTotal = 0;
  /*Número de paso que está dando en la instrucción*/
  int pasoActual = 0;
  /*Indica si el motor se encuentra ejecutando una instrucción de rotación*/
  bool estaRotando = false;

  /*Sentido en el que gira el motor. 0 parado, 1 sentido horario, -1 sentido antihorario*/
  enum class SentidoGiro {
    Parado = 0,
    Horario = 1,
    Antihorario = -1
  } sentidoGiro;

  /*Tiempo entre paso y paso medido en milisegundos*/
  int tiempoPaso = 1;

  int _mode = 0;
  int _steps = -1;
  int _delayMs = 1;
  int _engineStep = 0;
  int _currentStep = 0;
  int _rotationDirection = 0;

  MotorPasos(int pinA, int pinB, int pinC, int pinD);

  /**
   * @brief Aplica tensión al pin correspondiente según el paso actual del motor.
   *
   * Este método activa el pin adecuado en función del valor de `secuenciaPaso`,
   * estableciendo el pin en alto (HIGH) para energizar la bobina correspondiente
   * al paso. Cada paso sigue una secuencia que permite el giro controlado del motor.
   *
   * - Paso 1: Activa `pinD`
   * - Paso 2: Activa `pinB`
   * - Paso 3: Activa `pinC`
   * - Paso 4: Activa `pinA`
   */
  void energizarBobinaActual();

  /**
   * @brief Actualiza el número de paso en función del sentido de giro del motor.
   *
   * Este método incrementa o decrementa la variable `secuenciaPaso` según el
   * sentido de giro especificado. Si el sentido es horario, avanza al siguiente
   * paso y reinicia en 1 cuando se excede el máximo (4). Si el sentido es
   * antihorario, retrocede al paso anterior y reinicia en 4 cuando es menor a 1.
   */
  void siguientePaso();

  /**
   * @brief Libera la tensión de todos los pines y deja
   * el motor en punto muerto.
   */
  void parar();

  /**
   * @brief
   *
   * @param cantidadPasos numero de pasos a dar
   */
  void rotarPasos(int cantidadPasos);

  /**
   * @brief Establece el sentido de giro del motor basado en el número de pasos.
   *
   * Esta función determina el sentido de giro del motor según el valor
   * del parámetro `pasos`. Utiliza la función `std::copysign` para
   * establecer el signo del número. Dependiendo del signo, el motor
   * se configura para girar en sentido horario, antihorario o se
   * detiene.
   *
   * @param pasos Un entero que representa el número de pasos que
   *              determinará el sentido de giro.
   *              - Un valor positivo indica sentido horario.
   *              - Un valor negativo indica sentido antihorario.
   *              - Un valor cero indica que el motor debe estar parado.
   */
  void estableceSentidoGiro(int pasos);


};
