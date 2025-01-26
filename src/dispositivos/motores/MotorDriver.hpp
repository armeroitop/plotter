#pragma once

#include <wiringPi.h>

#include <iostream>

#include "../../utilidades/Fisicas.hpp"

struct MotorDriver : Fisicas {
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

  Fisicas fisica;

  /*Sentido en el que gira el motor. 0 parado, 1 sentido horario, -1 sentido
   * antihorario*/
  enum class SentidoGiro {
    Parado = 0,
    Horario = 1,
    Antihorario = -1
  } sentidoGiro;

  /*Tiempo entre paso y paso medido en milisegundos*/
  int tiempoPaso = 1;

  explicit MotorDriver() : Fisicas() {}

  /**
   * @brief Actualiza el número de paso en función del sentido de giro del
   * motor.
   *
   * Este método incrementa o decrementa la variable `secuenciaPaso` según el
   * sentido de giro especificado. Si el sentido es horario, avanza al siguiente
   * paso y reinicia en 1 cuando se excede el máximo (4). Si el sentido es
   * antihorario, retrocede al paso anterior y reinicia en 4 cuando es menor
   * a 1.
   */
  virtual void siguientePaso() = 0;

  /**
   * @brief Libera la tensión de todos los pines y deja
   * el motor en punto muerto.
   */
  virtual void parar() = 0;

  /**
   * @brief
   *
   * @param cantidadPasos numero de pasos a dar
   */
  virtual void rotarPasos(int cantidadPasos) = 0;

  virtual void rotar() = 0;

  /**
   * @brief Aplica tensión al pin correspondiente según el paso actual del
   * motor.
   *
   * Este método activa el pin adecuado en función del valor de `secuenciaPaso`,
   * estableciendo el pin en alto (HIGH) para energizar la bobina
   * correspondiente al paso. Cada paso sigue una secuencia que permite el giro
   * controlado del motor.
   *
   * - Paso 1: Activa `pinD`
   * - Paso 2: Activa `pinB`
   * - Paso 3: Activa `pinC`
   * - Paso 4: Activa `pinA`
   */
  virtual void energizarBobinaActual() = 0;

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
  virtual void estableceSentidoGiro(int pasos) = 0;

  /**
   * @brief Verifica si se han completado todos los pasos de la instrucción.
   *
   * Este método compara el número actual de pasos (`pasoActual`) con el número
   * total de pasos requeridos (`pasosTotal`). Devuelve `true` si se han
   * completado todos los pasos, o `false` en caso contrario.
   *
   * @return `true` si el número de pasos actuales es mayor o igual al total de
   * pasos, `false` en caso contrario.
   */
  virtual bool haCompletadoPasos() = 0;

  virtual ~MotorDriver() = default;
};
