#include "Gcode.hpp"
#include <sstream>
#include <iostream>

Gcode::Gcode(PlanificadorDeMovimiento& planificador) : planificador(planificador) { }

void Gcode::interpretar(const std::string& instruccion) {
    std::istringstream entrada(instruccion);
    std::string comando;
    entrada >> comando;

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
        moverA(x, y);
    } else {
        std::cerr << "Comando no reconocido: " << comando << std::endl;
    }
}

void Gcode::moverA(int x, int y) {
    printf("Gcode X: %i, Y: %i  \n", x, y);
    planificador.moverA(x, y);
}
