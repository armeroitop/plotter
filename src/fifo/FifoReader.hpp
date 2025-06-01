#pragma once

#include "GcodeQueue.hpp"
#include <string>
#include <thread>
#include <atomic>

class FifoReader {
public:
    FifoReader(const std::string& fifoPath, GcodeQueue& queue);
    ~FifoReader();

    void start();
    void stop();

private:
    void readLoop();

    std::string fifoPath;
    GcodeQueue& queue;
    std::thread readerThread;
    std::atomic<bool> running;
};