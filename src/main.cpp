#include <stdio.h>
#include <wiringPi.h>

#include "control/PlanificadorDeMovimiento.hpp"
#include "dispositivos/motores/DRV8825Driver.hpp"
#include "dispositivos/motores/L298NDriver.hpp"
#include "dispositivos/motores/MotorDriver.hpp"
#include "include/config.hpp"

int main(void) {
    printf("Inicio del programa\n");
    wiringPiSetup();

    DRV8825Driver motorIz(config::MP1_step_pin, config::MP1_dir_pin,
                          config::MP1_enable_pin);

    DRV8825Driver motorDr(config::MP2_step_pin, config::MP2_dir_pin,
                          config::MP2_enable_pin);

    motorIz.nombre = "motorIz";
    motorDr.nombre = "motorDr";

    PlanificadorDeMovimiento planificador;
    planificador.setMotores(motorIz, motorDr);

    planificador.moverA(9, 10); // vamos a suponer que salimos desde 0 y tenemos que llegar a 10

    printf("Fin del programa\n");
    return 0;
}