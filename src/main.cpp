// src/main.cpp
#include <iostream>
#include <csignal>
#include <atomic>
#include <wiringPi.h>
#include <string>

#include "fifo/FifoReader.hpp"
#include "fifo/GcodeExecutor.hpp"
#include "fifo/GcodeQueue.hpp"
#include "fifo/FifoWriter.hpp"

#include "control/PlanificadorDeMovimiento.hpp"
#include "dispositivos/motores/DRV8825Driver.hpp"
#include "dispositivos/motores/MotorDriver.hpp"
#include "dispositivos/interruptores/FinalDeCarrera.hpp"
#include "dispositivos/servo/ServoBoli.hpp"
#include "parseador/Gcode.hpp"
#include "include/config.hpp"

std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "\n[main] Señal recibida (" << signum << "), cerrando...\n";
    running = false;
}

int main() {
    std::signal(SIGINT, signalHandler);
    wiringPiSetup();

    // === Configuración ===
    ServoBoli servoBoli(config::pin_servoBoli);

    DRV8825Driver motorX(config::MP1_step_pin, config::MP1_dir_pin, config::MP1_enable_pin);
    motorX.nombre = "motorX";
    DRV8825Driver motorY(config::MP2_step_pin, config::MP2_dir_pin, config::MP2_enable_pin);
    motorY.nombre = "motorY";

    PlanificadorDeMovimiento planificador;
    planificador.setMotores(motorX, motorY);

    FinalDeCarrera finXmin(config::pin_finXmin);
    FinalDeCarrera finXmax(config::pin_finXmax);
    FinalDeCarrera finYmin(config::pin_finYmin);
    FinalDeCarrera finYmax(config::pin_finYmax);
    planificador.setFinalesDeCarrera(finXmin, finXmax, finYmin, finYmax);

    Gcode gcode(planificador, servoBoli);

    // === Cola, FIFO, y ejecutor ===
    GcodeQueue queue;
    FifoReader fifoReader("/tmp/gcode_pipe", queue);
    GcodeExecutor executor(queue, gcode);

    fifoReader.start();
    executor.start();

    std::cout << "[main] Esperando comandos en /tmp/gcode_pipe (Ctrl+C para salir)\n";

    FifoWriter writer("/tmp/plotter_out");
    writer.start();

    if (writer.isReady()) {
        writer.write("Inicio correcto del planificador");
    } else {
        std::cerr << "No se pudo iniciar la comunicación con el FIFO." << std::endl;
    }


    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    fifoReader.stop();
    executor.stop();

    std::cout << "[main] Sistema detenido correctamente.\n";
    return 0;
}
