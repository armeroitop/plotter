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
        std::cout << "Modo ABSOLUTO activado" << std::endl;
    } else if (comando == "G91") {
        modoRelativo = true;
        std::cout << "Modo RELATIVO activado" << std::endl;
    } else if (comando == "M1") {
        servoBoli.levantar();
        std::cout << "Levantando el boli" << std::endl;
    } else if (comando == "M2") {
        servoBoli.bajar();
        std::cout << "Bajando el boli" << std::endl;
    } else if (comando == "M999") {
        servoBoli.liberar();
        std::cout << "[GCODE] Liberar tensión en servo del boli" << std::endl;
    } else {
        std::cerr << "Comando no reconocido: " << comando << std::endl;
    }
}

