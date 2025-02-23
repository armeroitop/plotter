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

int main(int argc, char* argv []) {
    printf("Inicio del programa\n");
    wiringPiSetup();

    std::ifstream archivoGcode(argv[1]);
    if (!archivoGcode.is_open()) {
        printf("No se pudo abrir el archivo: %s\n", argv[1]);
        return 1;
    }
    // Configuración de los motores
    DRV8825Driver motorX(config::MP1_step_pin, config::MP1_dir_pin,
                          config::MP1_enable_pin);
    motorX.nombre = "motorX";

    DRV8825Driver motorY(config::MP2_step_pin, config::MP2_dir_pin,
                          config::MP2_enable_pin);
    motorY.nombre = "motorY";

    // Configuración del planificador de movimiento
    PlanificadorDeMovimiento planificador;
    planificador.setMotores(motorX, motorY);

    // Configuración de los finales de carrera
    FinalDeCarrera finXmin(config::pin_finXmin);
    FinalDeCarrera finXmax(config::pin_finXmax);
    FinalDeCarrera finYmin(config::pin_finYmin);
    FinalDeCarrera finYmax(config::pin_finYmax);
    planificador.setFinalesDeCarrera(finXmin, finXmax, finYmin, finYmax);

    // Interpretación del archivo Gcode
    Gcode gcode(planificador);
    std::string linea;
    while (std::getline(archivoGcode, linea)) {
        gcode.interpretar(linea);
        // Verificar si se ha activado la parada de emergencia
        if (planificador.esParadaDeEmergencia()) {
            // Notificar al operador
            std::cout << "Parada de emergencia activada. Verifique los finales de carrera." << std::endl;

            // Esperar hasta que se desactive la parada de emergencia
            while (planificador.esParadaDeEmergencia()) {
                delay(100); // Esperar 100 ms antes de verificar nuevamente
            }

            // Notificar al operador que se ha desactivado la parada de emergencia
            std::cout << "Parada de emergencia desactivada. Reanudando operaciones." << std::endl;
        }
        delay(1000);
    }

    archivoGcode.close();

    printf("Fin del programa\n");
    return 0;
}