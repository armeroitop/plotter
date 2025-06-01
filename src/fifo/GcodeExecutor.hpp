#pragma once

#include "GcodeQueue.hpp"
#include "parseador/Gcode.hpp"

#include <thread>
#include <atomic>

class GcodeExecutor {
    public:
    GcodeExecutor(GcodeQueue& queue, Gcode& gcode);
    ~GcodeExecutor();

    void start();
    void stop();

    private:
    void executionLoop();

    GcodeQueue& queue;
    Gcode& gcode;
    std::thread executorThread;
    std::atomic<bool> running;
};


