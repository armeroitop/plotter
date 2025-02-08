#include "Fisicas.hpp"

Fisicas::Fisicas() : tiempoDePaso(10000), tiempoDeInicio(0)  {
    ponTiempoDeInicio();
}

void Fisicas::ponTiempoDePaso(int ms) { tiempoDePaso = ms; }

void Fisicas::ponTiempoDeInicio() {
    tiempoDeInicio = obtenerTiempoActualMs();
}

int64_t Fisicas::obtenerTiempoActualMs() {
  auto tiempoAhora = std::chrono::steady_clock::now();
  //TODO Fet! cambiar std::chrono::milliseconds por std::chrono::microseconds
  auto tiempoMs = std::chrono::duration_cast<std::chrono::microseconds>(
                      tiempoAhora.time_since_epoch())
                      .count();
  return tiempoMs;
}

bool Fisicas::haPasadoTiempoDePaso() {
   // printf("En rotar mi paso es%d \n", tiempoDeInicio);

  return (obtenerTiempoActualMs() > (tiempoDeInicio + tiempoDePaso));
}



