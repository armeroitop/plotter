#include "Fisicas.hpp"

Fisicas::Fisicas() : tiempoDePaso(10000), tiempoDeInicio(0) {
    ponTiempoDeInicio();
}

void Fisicas::ponTiempoDePaso(int64_t ms) { tiempoDePaso = ms; }

void Fisicas::ponTiempoDeInicio() {
    tiempoDeInicio = obtenerTiempoActualMs();
}

int64_t Fisicas::obtenerTiempoActualMs() {
    auto tiempoAhora = std::chrono::steady_clock::now();
    
    auto tiempoMicros = std::chrono::duration_cast<std::chrono::microseconds>(
                        tiempoAhora.time_since_epoch())
        .count();
    return tiempoMicros;
}

bool Fisicas::haPasadoTiempoDePaso() {
    // printf("En rotar mi paso es%d \n", tiempoDeInicio);

    return (obtenerTiempoActualMs() > (tiempoDeInicio + tiempoDePaso));
}



