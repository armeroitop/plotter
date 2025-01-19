#include <stdio.h>
#include <wiringPi.h>

#include "componentes/DRV8825Driver.hpp"
#include "componentes/L298NDriver.hpp"
#include "componentes/MotorDriver.hpp"
#include "include/config.hpp"

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.

#define LED 16

// MotorPasos m1(1, 4, 5, 6);
// L298NDriver driverL(config::MP1_Pin1, config::MP1_Pin2,
// config::MP1_Pin3,config::MP1_Pin4);
DRV8825Driver driverL(config::step_pin, config::dir_pin, config::enable_pin);

MotorDriver& m1 = driverL;

int main(void) {
  printf("Raspberry Pi blink\n");

  wiringPiSetup();

  m1.rotarPasos(400);
  m1.ponTiempoDePaso(5);

  while (m1.estaRotando) {
    m1.rotar();
    // printf("Paso actual %d \n", m1.pasoActual);
  }

  m1.rotarPasos(-400);
 

  while (m1.estaRotando) {
    m1.rotar();
    // printf("Paso actual %d \n", m1.pasoActual);
  }

  m1.rotarPasos(400);
  

  while (m1.estaRotando) {
    m1.rotar();
    // printf("Paso actual %d \n", m1.pasoActual);
  }
  m1.rotarPasos(-400);
  

  while (m1.estaRotando) {
    m1.rotar();
    // printf("Paso actual %d \n", m1.pasoActual);
  }

  m1.parar();

  printf("Fin del programa\n");
  return 0;
}