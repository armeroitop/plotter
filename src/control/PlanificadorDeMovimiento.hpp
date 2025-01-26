#pragma once
#include "../dispositivos/motores/DRV8825Driver.hpp"

struct PlanificadorDeMovimiento {
  MotorDriver* p_motorX;
  MotorDriver* p_motorY;
  int x_actual = 0, y_actual = 0;

  float velocidadUnitariaMax = 0;
  float velocidadX = 0;
  float velocidadY = 0;

  float velocidadAngularMax = 0.2f;  // pasos/milisegundos

  explicit PlanificadorDeMovimiento();

  void setMotores(MotorDriver& motorX, MotorDriver& motorY);

  void setVelocidadMaxima(float velocidad);
  void setAceleracionMaxima(float aceleracion);

  // Define cuántos pasos por unidad (e.g., mm o grados).
  void setResolucionPaso(float pasosPorUnidad);

  // Calcula trayectorias y activa los motores.
  void moverA(float x, float y);

  void moverMotorX(int pasos, bool direccion);  // Avanza pasos en X.
  void moverMotorY(int pasos, bool direccion);  // Avanza pasos en Y.

  void calcularPasos(float deltaX, float deltaY, int& pasosMotorX,
                     int& pasosMotorY);

  void detener();   // Detiene todos los motores.
  void pausar();    // Pausa el movimiento actual.
  void reanudar();  // Reanuda después de una pausa.

  bool movimientoEnCurso();  // Devuelve si hay un movimiento en proceso.
  void obtenerPosicion(float& x, float& y);  // Retorna la posición actual.

  // Permite suavizar el arranque y la detención utilizando aceleración y
  // deceleración.
  float calcularVelocidadInterpolada(float distancia, float velocidadMaxima);

  // Configura la posición actual como (0,0).
  void establecerOrigen();

  // Detecta los límites mecánicos y los configura.
  void calibrar();
};
