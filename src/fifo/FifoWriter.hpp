#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <fstream>
#include <fcntl.h>  
#include <mutex>

class FifoWriter {

    public:
    static std::string fifoPath;
    static std::ofstream fifoStream;
    static int fd;
    static bool started;
    static std::mutex writeMutex;

    static void start(const std::string& path);
    static void stop();
    static void write(const std::string& message);
    static bool isReady();


    FifoWriter() = delete;
    ~FifoWriter();


};

