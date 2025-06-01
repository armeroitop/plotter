// src/fifo/GcodeExecutor.cpp
#include "GcodeExecutor.hpp"
#include <iostream>
#include <chrono>

GcodeExecutor::GcodeExecutor(GcodeQueue& queue, Gcode& gcode)
    : queue(queue), gcode(gcode), running(false) {}

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

void GcodeExecutor::executionLoop() {
    while (running) {
        std::string linea = queue.pop(); // Espera si la cola está vacía
        std::cout << "[Executor] Ejecutando: " << linea << std::endl;
        gcode.interpretar(linea);

        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Opcional
    }
}
