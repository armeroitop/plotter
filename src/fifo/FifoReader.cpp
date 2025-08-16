#include "FifoReader.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm> // para std::remove

FifoReader::FifoReader(const std::string& fifoPath, GcodeQueue& queue)
    : fifoPath(fifoPath), queue(queue), running(false) { }

FifoReader::~FifoReader() {
    stop();

    if (dummyWriterFd != -1) {
        close(dummyWriterFd);
    }
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

    // Mantener abierto un descriptor de escritura "dummy"
    dummyWriterFd = open(fifoPath.c_str(), O_WRONLY | O_NONBLOCK);
    if (dummyWriterFd == -1) {
        perror("dummy open");
    }

    readerThread = std::thread(&FifoReader::readLoop, this);
}

void FifoReader::stop() {
    running = false;
    if (readerThread.joinable()) {
        readerThread.join();
    }

     if (dummyWriterFd != -1) {
        close(dummyWriterFd);
        dummyWriterFd = -1;
    }
}

void FifoReader::readLoop() {
    std::ifstream fifo(fifoPath);
    if (!fifo.is_open()) {
        std::cerr << "[FifoReader] Error abriendo el FIFO.\n";
        return;
    }

    std::string linea;
    while (running) {
        if (std::getline(fifo, linea)) {
            if (!linea.empty()) {
                // Si es una linea de procesar un archivo, lo leemos y enviamos la queue todo su contenido
                if (linea[0] == '@') {
                    std::string rutaArchivo = linea.substr(1);
                     // Trim inicial - le quita espacios en blanco al inicio
                    rutaArchivo.erase(0, rutaArchivo.find_first_not_of(" \t\n\r"));
                    
                    std::ifstream archivoGcode(rutaArchivo);
                    if (!archivoGcode.is_open()) {
                        std::cerr << "[FifoReader] Error abriendo archivo: " << rutaArchivo << std::endl;
                        continue;
                    }

                    std::string lineaArchivo;
                    while (std::getline(archivoGcode, lineaArchivo)) {
                        if (!lineaArchivo.empty()) {
                            std::cout << "[FifoReader] G-code de archivo: " << lineaArchivo << std::endl;
                            queue.push_back(lineaArchivo);
                        }
                    }

                    archivoGcode.close();
                // Si es una línea normal de G-code, la procesamos directamente
                } else {
                    // Parada de emergencia - Le da prioridad a la parada de emergencia
                    if( linea == "M112"){
                        queue.push_front(linea); 
                    } else{
                        std::cout << "[FifoReader] G-code recibido: " << linea << std::endl;
                        queue.push_back(linea);
                    }

                }
            }
        } else {
            // Si EOF o error, esperar un poco y continuar
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            fifo.clear();  // limpiar estado de EOF
        }
    }

    fifo.close();
}


