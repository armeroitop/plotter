#pragma once

#include <string>
#include "../control/PlanificadorDeMovimiento.hpp"
#include "../dispositivos/servo/ServoBoli.hpp"


struct Gcode {
    Gcode(PlanificadorDeMovimiento& planificador, ServoBoli& servoBoli);
    void interpretar(const std::string& instruccion);

    private:
    PlanificadorDeMovimiento& planificador;
    ServoBoli& servoBoli;

    void moverA(int x, int y);
};
