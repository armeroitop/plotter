#include <stdio.h>
#include <wiringPi.h>

#include "control/PlanificadorDeMovimiento.hpp"
#include "dispositivos/motores/DRV8825Driver.hpp"
#include "dispositivos/motores/L298NDriver.hpp"
#include "dispositivos/motores/MotorDriver.hpp"
#include "include/config.hpp"

int main(void) {
  printf("Raspberry Pi blink\n");
  wiringPiSetup();

  DRV8825Driver driverL(config::MP1_step_pin, config::MP1_dir_pin,
                        config::MP1_enable_pin);

  DRV8825Driver driverR(config::MP2_step_pin, config::MP2_dir_pin,
                        config::MP2_enable_pin);

  // MotorDriver& m1 = driverL;
  // MotorDriver& m2 = driverR;

  PlanificadorDeMovimiento planificador;
  planificador.setMotores(driverL, driverR);

  planificador.moverA(
      10, 10);  // vamos a suponer que salimos desde 0 y tenemos que llegar a 10

  // m1.rotarPasos(200);
  // m1.ponTiempoDePaso(5);
  //
  // while (m1.estaRotando) {
  //   m1.rotar();
  //   // printf("Paso actual %d \n", m1.pasoActual);
  // }
  //
  // m1.parar();
  //
  printf("Fin del programa\n");
  return 0;
}