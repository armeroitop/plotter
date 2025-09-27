#pragma once

#include <string>
#include <deque>
#include <optional>
#include "../control/PlanificadorDeMovimiento.hpp"
#include "../dispositivos/servo/ServoBoli.hpp"


struct Gcode {
    Gcode(PlanificadorDeMovimiento& planificador, ServoBoli& servoBoli);
    void interpretar(const std::string& instruccion, std::deque<std::string>& bufferMovimientos);

    bool modoRelativo = false; // Modo de coordenadas relativas

    std::deque<std::string> listaMovimientos; // lookahead FIFO

    private:
    PlanificadorDeMovimiento& planificador;
    ServoBoli& servoBoli;


};
