#pragma once

#include <string>
#include "../control/PlanificadorDeMovimiento.hpp"
#include "../dispositivos/servo/ServoBoli.hpp"


struct Gcode {
    Gcode(PlanificadorDeMovimiento& planificador, ServoBoli& servoBoli);
    void interpretar(const std::string& instruccion);

    bool modoRelativo = false; // Modo de coordenadas relativas

    private:
    PlanificadorDeMovimiento& planificador;
    ServoBoli& servoBoli;


};
