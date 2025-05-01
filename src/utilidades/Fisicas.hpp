#pragma once
#include <chrono>

struct Fisicas {
  int tiempoDePaso;
  int64_t tiempoDeInicio;
  int64_t tiempoFinalizacion;

  // Resolución de pasos del motor (pasos por unidad de medida, e.g., mm)

  static inline constexpr float diametroPolea = 12.25f;  // en mm

  static inline constexpr float resolucionAngular = 1.8f;  // grados por paso

  /* Pasos por milímetro (pasos/mm) */
  inline static constexpr float resolucionPaso =
      360 / (diametroPolea * resolucionAngular * 3.14159);


  /* Milimetros recorridos por paso (mm/paso)*/
  inline static constexpr float distanciaPaso =
      (diametroPolea * resolucionAngular * 3.14159) / 360;

  Fisicas();
  void ponTiempoDePaso(int ms);

  /**
   * @brief Establece el tiempo de inicio al tiempo actual.
   *
   * Obtiene el tiempo actual en microsegundos llamando al método
   * `obtenerTiempoActualMs()` y lo asigna a la variable miembro
   * `tiempoDeInicio`.
   *
   * @note Este método debe ser llamado antes de realizar cualquier comparación
   * de tiempo con `haPasadoTiempoDePaso()` para asegurar que se tiene un punto
   * de referencia correcto.
   */
  void ponTiempoDeInicio();

  int64_t obtenerTiempoActualMs();

  /**
   * @brief Verifica si ha transcurrido el intervalo de tiempo de paso desde el
   * inicio.
   *
   * Compara el tiempo actual en microsegundos con el tiempo de
   * inicio más el intervalo especificado para el paso. Si el tiempo actual es
   * mayor, significa que el intervalo de paso ha transcurrido y devuelve
   * `true`. En caso contrario, devuelve `false`.
   *
   * @return `true` si el tiempo actual ha superado el tiempo de inicio más el
   * intervalo de tiempo de paso.
   * @return `false` si aún no ha transcurrido el intervalo de tiempo de paso.
   *
   * @note Este método asume que `tiempoDeInicio` y `tiempoDePaso` son
   * configurados correctamente antes de su llamada, ya que no se actualizan
   * dentro del método.
   */
  bool haPasadoTiempoDePaso();
};
