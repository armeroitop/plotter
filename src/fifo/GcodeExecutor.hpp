#pragma once

#include "GcodeQueue.hpp"
#include "parseador/Gcode.hpp"

#include <thread>
#include <atomic>
#include <deque>
#include <string>

class GcodeExecutor {
    public:
    GcodeExecutor(GcodeQueue& queue, Gcode& gcode);
    ~GcodeExecutor();

    void start();
    void stop();

    private:
    void executionLoop();
    std::deque<std::string> bufferMovimientos; // solo comandos G1

    GcodeQueue& queue;
    Gcode& gcode;
    std::thread executorThread;
    std::atomic<bool> running;
};


