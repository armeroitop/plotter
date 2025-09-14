#include "cargaParametros.hpp"

// Con esto limpiamos las advertencias de nlohmann/json cuando compilamos con -Wall
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic pop

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>


using json = nlohmann::json;

Parametros::Parametros()
    : velocidadMax(0), aceleracion(0), puertoSerie(""), ancho_mm(0), alto_mm(0) { }

Parametros& Parametros::getInstance() {
    // TODO: Insertar una instrucción "return" aquí
    static Parametros instancia;
    return instancia;
}

void Parametros::cargarParametrosJSON(const std::string& ruta) {
    std::ifstream f(ruta);
    if (!f.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo de parametros: " + ruta);
    }

    json j;
    f >> j;

    velocidadMax = j.value("velocidadMax", 100);  // valor por defecto si no existe
    aceleracion = j.value("aceleracion", 100);
    puertoSerie = j.value("puertoSerie", "/dev/ttyUSB0");
    ancho_mm = j.value("ancho_mm", 200); // valor por defecto si no existe
    alto_mm = j.value("alto_mm", 200);

}