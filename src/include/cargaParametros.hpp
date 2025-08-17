#include <iostream>
#include <fstream>

// Con esto limpiamos las advertencias de nlohmann/json cuando compilamos con -Wall
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <nlohmann/json.hpp>
#pragma GCC diagnostic pop

using json = nlohmann::json;

struct Parametros {
    int velocidadMax;
    int aceleracion;
    std::string puertoSerie;
};

Parametros cargarParametros(const std::string& ruta) {
    std::ifstream f(ruta);
    if (!f.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo de parametros: " + ruta);
    }

    json j;
    f >> j;

    Parametros parametro;
    parametro.velocidadMax = j.value("velocidadMax", 100);  // valor por defecto si no existe
    parametro.aceleracion  = j.value("aceleracion", 100);
    parametro.puertoSerie  = j.value("puertoSerie", "/dev/ttyUSB0");

    return parametro;
}