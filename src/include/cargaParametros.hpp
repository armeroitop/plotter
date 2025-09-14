#pragma once

#include <string>

class Parametros {
    private:

    // Propiedades de configuración
    int velocidadMax;
    int aceleracion;
    std::string puertoSerie;
    int ancho_mm;
    int alto_mm;

    Parametros();

    public:

    // Evitamos copias del Singleton
    Parametros(const Parametros&) = delete;
    Parametros& operator=(const Parametros&) = delete;

    // Acceso al único objeto
    static Parametros& getInstance();

    // Métodos de carga
    void cargarParametrosJSON(const std::string& ruta);

    // Getters
    int getVelocidadMax() const { return velocidadMax; }
    int getAceleracion() const { return aceleracion; }
    std::string getPuertoSerie() const { return puertoSerie; }
    int getAncho() const { return ancho_mm; }
    int getAlto() const { return alto_mm; }
};
