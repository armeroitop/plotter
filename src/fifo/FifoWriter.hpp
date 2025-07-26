#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <fstream>
#include <fcntl.h>  

class FifoWriter {

    public:
    std::string fifoPath;
    std::ofstream fifoStream;
    int fd = -1;
    bool started = false;

    void start();
    void stop();
    void write(const std::string& message);
    bool isReady() const;


    FifoWriter(const std::string& fifoPath);
    ~FifoWriter();


};

