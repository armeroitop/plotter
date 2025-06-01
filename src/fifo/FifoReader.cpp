#include "FifoReader.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

FifoReader::FifoReader(const std::string& fifoPath, GcodeQueue& queue)
    : fifoPath(fifoPath), queue(queue), running(false) {}

FifoReader::~FifoReader() {
    stop();
}

void FifoReader::start() {
    running = true;

    // Crear el FIFO si no existe
    if (access(fifoPath.c_str(), F_OK) == -1) {
        if (mkfifo(fifoPath.c_str(), 0666) != 0) {
            perror("mkfifo");
            return;
        }
    }

    readerThread = std::thread(&FifoReader::readLoop, this);
}

void FifoReader::stop() {
    running = false;
    if (readerThread.joinable()) {
        readerThread.join();
    }
}

void FifoReader::readLoop() {
    while (running) {
        std::ifstream fifo(fifoPath);
        if (!fifo.is_open()) {
            std::cerr << "[FifoReader] Error abriendo el FIFO.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        std::string line;
        while (std::getline(fifo, line) && running) {
            if (!line.empty()) {
                queue.push(line);
                std::cout << "[FifoReader] G-code recibido: " << line << std::endl;
            }
        }

        fifo.close();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}