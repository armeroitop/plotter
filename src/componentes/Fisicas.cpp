#include "Fisicas.hpp"

Fisicas::Fisicas() : tiempoDePaso(10), tiempoDeInicio(0)  {
    ponTiempoDeInicio();
}

void Fisicas::ponTiempoDePaso(int ms) { tiempoDePaso = ms; }

void Fisicas::ponTiempoDeInicio() {
    tiempoDeInicio = obtenerTiempoActualMs();
}

int64_t Fisicas::obtenerTiempoActualMs() {
  auto tiempoAhora = std::chrono::steady_clock::now();
  auto tiempoMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                      tiempoAhora.time_since_epoch())
                      .count();
  return tiempoMs;
}

bool Fisicas::haPasadoTiempoDePaso() {
   // printf("En rotar mi paso es%d \n", tiempoDeInicio);

  return (obtenerTiempoActualMs() > (tiempoDeInicio + tiempoDePaso));
}
