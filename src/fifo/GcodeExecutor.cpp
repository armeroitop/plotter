// src/fifo/GcodeExecutor.cpp
#include "GcodeExecutor.hpp"
#include <iostream>
#include <chrono>
#include "fifo/FifoWriter.hpp"


GcodeExecutor::GcodeExecutor(GcodeQueue& queue, Gcode& gcode)
    : queue(queue), gcode(gcode), running(false) { }

GcodeExecutor::~GcodeExecutor() {
    stop();
}

void GcodeExecutor::start() {
    running = true;
    executorThread = std::thread(&GcodeExecutor::executionLoop, this);
}

void GcodeExecutor::stop() {
    running = false;
    if (executorThread.joinable()) {
        executorThread.join();
    }
}

/**
 * @brief Bucle principal de ejecución del GcodeExecutor.
 *
 * Extrae comandos G-code de la cola de manera secuencial y los interpreta.
 * Si el FIFO de escritura está disponible, registra el inicio y el resultado de la interpretación.
 * El bucle se mantiene activo mientras la variable 'running' sea verdadera.
 * Hace una pausa breve entre comandos para evitar saturar el sistema.
 */
void GcodeExecutor::executionLoop() {
    while (running) {
        std::string linea = queue.pop(); // Espera si la cola está vacía
        std::cout << "[Executor] Ejecutando: " << linea << std::endl;

        // Creamos un buffer con el movimiento actual y el siguiente 
        if (linea.rfind("G1", 0) == 0
            || linea.rfind("G01", 0) == 0
            || linea.rfind("G02", 0) == 0
            || linea.rfind("G03", 0) == 0) {

                bufferMovimientos.push_back(linea);

            if (!queue.empty()) {
                std::string siguiente = queue.peek();
                if (siguiente.rfind("G1", 0) == 0
                    || siguiente.rfind("G01", 0) == 0
                    || siguiente.rfind("G02", 0) == 0
                    || siguiente.rfind("G03", 0) == 0 ) {

                        bufferMovimientos.push_back(siguiente);
                }
            }
        }

        if (FifoWriter::isReady()) {
            FifoWriter::write("[Executor] Ejecutando: " + linea);
            gcode.interpretar(linea, bufferMovimientos);
            FifoWriter::write("[Executor] Linea interpretada: " + linea);
        } else {
            std::cerr << "[Executor] No se ha interpretado nada" << std::endl;
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Opcional
    }
}
