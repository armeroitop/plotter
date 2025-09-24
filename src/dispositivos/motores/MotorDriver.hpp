#pragma once

#include <wiringPi.h>

#include <iostream>

#include "../../utilidades/Fisicas.hpp"

struct MotorDriver : Fisicas {

  /*Nombre del motor*/
  std::string nombre= "";

  /*Número de pasos por una vuelta*/
  const int npasos = 0;

  /*Secuencia dentro de un paso en el que se encuentra actualmente*/
  int secuenciaPaso = 0;

  /*Número de pasos secuenciales a dar por instrucción*/
  int totalPasosSecuenciales = 0;
  
  /*Número de pasos a dar por instrucción*/
  int totalPasos = 0;

  /*Número de paso secuencial que está dando en la instrucción. No es el numero de paso*/
  int pasoActual = 0;

  /*Indica si el motor se encuentra ejecutando una instrucción de rotación*/
  bool estaRotando = false;

  /*Sentido en el que gira el motor. 0 parado, 1 sentido horario, -1 sentido
   * antihorario*/
  enum class SentidoGiro {
    Parado = 0,
    Horario = 1,
    Antihorario = -1
  } sentidoGiro;

  /*Tiempo entre paso y paso medido en microsegundos*/
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
  virtual void siguienteSecuencia() = 0;

  /**
   * @brief Libera la tensión de todos los pines y deja
   * el motor en punto muerto.
   */
  virtual void parar() = 0;

  virtual void arrancar() = 0;

  /**
   * @brief
   *
   * @param cantidadPasos numero de pasos a dar
   */
  virtual void rotarPasos(int cantidadPasos) = 0;

  virtual void rotar() = 0;

  virtual void detenerSiCompletado() = 0;
  
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
   * total de pasos requeridos (`totalPasosSecuenciales`). Devuelve `true` si se han
   * completado todos los pasos, o `false` en caso contrario.
   *
   * @return `true` si el número de pasos actuales es mayor o igual al total de
   * pasos, `false` en caso contrario.
   */
  virtual bool haCompletadoPasos() = 0;

  

  virtual ~MotorDriver() = default;
};
