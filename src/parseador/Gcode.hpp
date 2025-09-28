#pragma once

#include <string>
#include <deque>
#include <optional>
#include "../control/PlanificadorDeMovimiento.hpp"
#include "../dispositivos/servo/ServoBoli.hpp"


struct Gcode {
    Gcode(PlanificadorDeMovimiento& planificador, ServoBoli& servoBoli);
    void interpretar(const std::string& instruccion, std::deque<std::string>& bufferMovimientos);
    
    // Método para extraer coordenadas X e Y de un comando G1
    std::pair<float, float> extraerCoordenadas(const std::string& comandoG1);

    bool modoRelativo = false; // Modo de coordenadas relativas

    std::deque<std::string> listaMovimientos; // lookahead FIFO

    private:
    PlanificadorDeMovimiento& planificador;
    ServoBoli& servoBoli;


};
