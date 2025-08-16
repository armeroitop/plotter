#include "Gcode.hpp"
#include <sstream>
#include <iostream>

Gcode::Gcode(PlanificadorDeMovimiento& planificador, ServoBoli& servoBoli)
    : planificador(planificador), servoBoli(servoBoli) { }

void Gcode::interpretar(const std::string& instruccion) {
    std::istringstream entrada(instruccion);
    std::string comando;
    entrada >> comando;

    std::cout << "[Gcode::interpretar] Recibido: " << instruccion << std::endl;

    if (comando == "G1") {
        int x = 0, y = 0;
        char eje;
        while (entrada >> eje) {
            if (eje == 'X') {
                entrada >> x;
            } else if (eje == 'Y') {
                entrada >> y;
            }
        }

        if (modoRelativo) {
            printf("anificador.moverRelativo X: %i, Y: %i  \n", x, y);
            planificador.moverRelativo(x, y);
        } else {
            printf("planificador.moverA X: %i, Y: %i  \n", x, y);
            planificador.moverA(x, y);
        }

    } else if (comando == "G90") {
        modoRelativo = false;
        std::cout << "[GCODE] Modo ABSOLUTO activado" << std::endl;
    } else if (comando == "G91") {
        modoRelativo = true;
        std::cout << "[GCODE] Modo RELATIVO activado" << std::endl;
    } else if (comando == "M1") {
        servoBoli.levantar();
        std::cout << "[GCODE] Levantando el boli" << std::endl;
    } else if (comando == "M2") {
        servoBoli.bajar();
        std::cout << "[GCODE] Bajando el boli" << std::endl;
    } else if (comando == "M999") {
        servoBoli.liberar();
        planificador.desactivarParadaDeEmergencia();
        std::cout << "[GCODE] Liberar tensión en servo del boli" << std::endl;
    } else if (comando == "M112") {
        servoBoli.liberar();
        planificador.activarParadaDeEmergencia();
        std::cout << "[GCODE] Parada de emergencia" << std::endl;
    } else {
        std::cerr << "Comando no reconocido: " << comando << std::endl;
    }
}

