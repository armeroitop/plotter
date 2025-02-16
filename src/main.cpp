#include <stdio.h>
#include <wiringPi.h>
#include <fstream>
#include <string>

#include "control/PlanificadorDeMovimiento.hpp"
#include "dispositivos/motores/DRV8825Driver.hpp"
#include "dispositivos/motores/L298NDriver.hpp"
#include "dispositivos/motores/MotorDriver.hpp"
#include "include/config.hpp"
#include "parseador/Gcode.hpp"

int main(int argc, char *argv[]) {
    printf("Inicio del programa\n");
    wiringPiSetup();

    std::ifstream archivoGcode(argv[1]);
    if (!archivoGcode.is_open()) {
        printf("No se pudo abrir el archivo: %s\n", argv[1]);
        return 1;
    }

    DRV8825Driver motorX(config::MP1_step_pin, config::MP1_dir_pin,
                          config::MP1_enable_pin);

    DRV8825Driver motorY(config::MP2_step_pin, config::MP2_dir_pin,
                          config::MP2_enable_pin);

    motorX.nombre = "motorX";
    motorY.nombre = "motorY";

    PlanificadorDeMovimiento planificador;
    planificador.setMotores(motorX, motorY);

    FinalDeCarrera finXmin(config::pin_finXmin);
    FinalDeCarrera finXmax(config::pin_finXmax);
    FinalDeCarrera finYmin(config::pin_finYmin);
    FinalDeCarrera finYmax(config::pin_finYmax);
    planificador.setFinalesDeCarrera(finXmin, finXmax, finYmin, finYmax);

    //planificador.moverA(param1, param2); // vamos a suponer que salimos desde 0 y tenemos que llegar a 10
    //planificador.moverA(-9, 10); // vamos a suponer que salimos desde 0 y tenemos que llegar a 10
    
    Gcode gcode(planificador);
    std::string linea;
    while (std::getline(archivoGcode, linea)) {
        gcode.interpretar(linea);
        delay(1000);
    }

    archivoGcode.close();

    printf("Fin del programa\n");
    return 0;
}