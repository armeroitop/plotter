#pragma once

namespace config {

//** Final de carrera Eje X menor ****************
constexpr int pin_finXmin      = 29;

//** Final de carrera Eje X mayor ****************
constexpr int pin_finXmax      = 29;

//** Final de carrera Eje Y menor ****************
constexpr int pin_finYmin      = 29;

//** Final de carrera Eje Y mayor ****************
constexpr int pin_finYmax      = 29;

//** Motor de pasos 1 - Driver DRV8825 ****************
constexpr int MP1_step_pin      = 1;
constexpr int MP1_dir_pin       = 4;
constexpr int MP1_enable_pin    = 5;

//** Motor de pasos 2 - Driver DRV8825 ****************
constexpr int MP2_step_pin      = 0;
constexpr int MP2_dir_pin       = 2;
constexpr int MP2_enable_pin    = 3;



}  // namespace config
