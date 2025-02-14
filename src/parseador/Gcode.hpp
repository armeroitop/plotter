#pragma once

#include <string>
#include "../control/PlanificadorDeMovimiento.hpp"


struct Gcode
{
    Gcode(PlanificadorDeMovimiento& planificador);
    void interpretar(const std::string& instruccion);

private:
    PlanificadorDeMovimiento& planificador;
    void moverA(int x, int y);
};
