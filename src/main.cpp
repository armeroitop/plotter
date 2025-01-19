#include <stdio.h>
#include <wiringPi.h>

#include "include/config.hpp"
#include "componentes/L298NDriver.hpp"
#include "componentes/DRV8825Driver.hpp"
#include "componentes/MotorDriver.hpp"

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.

#define LED 16

// MotorPasos m1(1, 4, 5, 6);
// L298NDriver driverL(config::MP1_Pin1, config::MP1_Pin2, config::MP1_Pin3,config::MP1_Pin4);
DRV8825Driver driverL(config::step_pin, config::dir_pin, config::enable_pin);

MotorDriver& m1 = driverL;


int main(void) {
  printf("Raspberry Pi blink\n");

  wiringPiSetup();
  pinMode(LED, OUTPUT);

  m1.rotarPasos(-200);
  m1.rotarPasos(200);
  m1.rotarPasos(-200);
  m1.rotarPasos(200);

  /*for (int i = 0; i < 100; i++) {
    printf("Vuelta numero %d \n", i);
    digitalWrite(LED, HIGH);  // On
    delay(500);               // mS
    digitalWrite(LED, LOW);   // Off
    delay(500);
  }*/

  //while (m1.estaRotando) {
    //m1.rotar();
    // printf("Paso actual %d \n", m1.pasoActual);
  //}
  /* m1.rotarPasos(100);

   while (m1.estaRotando) {
    m1.rotar();
    //printf("Paso actual %d \n", m1.pasoActual);
  }  */
  m1.parar();
  // m1.secuenciaPaso=3;
  // m1.energizarBobinaActual();

    printf("Fin del programa\n");
  return 0;
}