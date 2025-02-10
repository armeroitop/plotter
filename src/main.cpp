#include <stdio.h>
#include <wiringPi.h>

#include "control/PlanificadorDeMovimiento.hpp"
#include "dispositivos/motores/DRV8825Driver.hpp"
#include "dispositivos/motores/L298NDriver.hpp"
#include "dispositivos/motores/MotorDriver.hpp"
#include "include/config.hpp"

int main(int argc, char *argv[]) {
    printf("Inicio del programa\n");
    wiringPiSetup();

    if (argc != 3) {
        printf("Uso: %s <param1> <param2>\n", argv[0]);
        return 1;
    }

    int param1 = atoi(argv[1]);
    int param2 = atoi(argv[2]);

    DRV8825Driver motorX(config::MP1_step_pin, config::MP1_dir_pin,
                          config::MP1_enable_pin);

    DRV8825Driver motorY(config::MP2_step_pin, config::MP2_dir_pin,
                          config::MP2_enable_pin);

    motorX.nombre = "motorX";
    motorY.nombre = "motorY";

    PlanificadorDeMovimiento planificador;
    planificador.setMotores(motorX, motorY);

    planificador.moverA(param1, param2); // vamos a suponer que salimos desde 0 y tenemos que llegar a 10
    //planificador.moverA(-9, 10); // vamos a suponer que salimos desde 0 y tenemos que llegar a 10

    printf("Fin del programa\n");
    return 0;
}