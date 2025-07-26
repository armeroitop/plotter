#include "FifoWriter.hpp"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h> 


FifoWriter::FifoWriter(const std::string& fifoPath)
    :fifoPath(fifoPath) { }

FifoWriter::~FifoWriter() {
    if (fifoStream.is_open()) {
        fifoStream.close();
    }

    if (fd != -1) {
        close(fd);
    }
}

void FifoWriter::start() {
    if (started) return;
    started = true;

    // Crear el FIFO si no existe
    if (access(fifoPath.c_str(), F_OK) == -1) {
        if (mkfifo(fifoPath.c_str(), 0666) != 0) {
            perror("mkfifo");
            return;
        }
    }

    // Abrir en modo no bloqueante (solo si hay lector) -> O_WRONLY
    // Nota: lo he dejado con O_RDWR por si no hay lector, así no se bloquea
    fd = open(fifoPath.c_str(), O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        perror("[FifoWriter] Error al abrir FIFO");
    } else {
        std::cout << "[FifoWriter] FIFO abierto correctamente: " << fifoPath << std::endl;
    }

    /* fifoStream.open(fifoPath, std::ios::out);
    if (!fifoStream.is_open()) {
        std::cerr << "[FifoWriter] Error al abrir el FIFO para escritura.\n";
    }*/
}

// Método para escribir un mensaje en el FIFO
void FifoWriter::write(const std::string& message) {

    if (fd == -1) {
        std::cerr << "[FifoWriter] FIFO no está abierto para escritura.\n";
        return;
    }

    /*if (fifoStream.is_open()) {
        fifoStream << message << std::endl;
    } else {
        std::cerr << "Error al abrir FIFO para escritura: " << fifoPath << std::endl;
    }*/

    std::string mensajeFinal = message + "\n";
    ssize_t result = ::write(fd, mensajeFinal.c_str(), mensajeFinal.size());

    if (result == -1) {
        perror("[FifoWriter] Error al escribir en FIFO");
    } else {
        std::cout << "[FifoWriter] Mensaje escrito en FIFO: " << message << std::endl;
    }
}


bool FifoWriter::isReady() const {
    return fd != -1;
}
