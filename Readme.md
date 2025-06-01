# Plotter CNC con Banana Pi M2 Zero

Este proyecto implementa un plotter CNC controlado por una Banana Pi M2 Zero, capaz de interpretar archivos G-code para mover motores paso a paso y controlar un servo que sube y baja un bolígrafo.

## Características

- **Interpretación de G-code**: Soporta comandos de movimiento en X e Y (`G1`) y control del bolígrafo (`M1` para levantar, `M2` para bajar).
- **Control de motores**: Utiliza drivers DRV8825 y L298N para motores paso a paso.
- **Control de servo**: Sube y baja el bolígrafo mediante un servo controlado por PWM por software.
- **Finales de carrera**: Soporte para finales de carrera en ambos ejes.
- **Configuración flexible**: Pines y parámetros configurables desde el archivo `config.hpp`.

## Requisitos

- Banana Pi M2 Zero con Armbian o similar.
- Bibliotecas: [BPI-WiringPi2](https://github.com/BPI-SINOVOIP/BPI-WiringPi2), g++.
- Drivers de motor DRV8825 y/o L298N.
- Servo compatible con PWM.
- Motores paso a paso y finales de carrera.

## Instalación

1. **Instalar la libreria BPI-WiringPi2**

   Seguir el procedimiento qu se describe en https://forum.banana-pi.org/t/banana-pi-bpi-m2-zero-wiringpi2/5517/7

2. **Clona el repositorio:**
   ```bash
   git clone https://github.com/armeroitop/plotter.git 
   cd plotter-cnc
   ```

3. **Compila el proyecto:**
   ```bash
   make
   ```

4. **Conecta los motores, servo y finales de carrera según la configuración de pines en `src/include/config.hpp`.**

## Uso

1. **Prepara un archivo G-code** con comandos como:
   ```
   G1 X10 Y20
   M2
   G1 X30 Y40
   M1
   G1 X50 Y60
   ```

2. **Conecta los motores, servo y finales de carrera según la configuración de pines en `src/include/config.hpp`.**

## Ejecución
### Ejecutar en modo FIFO (modo interactivo recomendado)

```bash
sudo ./plotter
```

Una vez iniciado, puedes enviar comandos así:

#### Enviar un comando G-code manualmente:

```bash
echo "G1 X10 Y10" > /tmp/gcode_pipe
```

#### Enviar un archivo `.gcode` completo:

```bash
echo "@/home/davidBPI/gcodes/circulo.gcode" > /tmp/gcode_pipe
```

El programa leerá y ejecutará todas las líneas del archivo enviado.

3. **Observa cómo el plotter mueve el bolígrafo y ejecuta los movimientos especificados.**

## Estructura del proyecto

* `src/main.cpp`: Punto de entrada del programa.
* `src/parseador/Gcode.cpp`: Interpretación de comandos G-code.
* `src/control/PlanificadorDeMovimiento.cpp`: Lógica de planificación de movimientos.
* `src/dispositivos/motores/`: Drivers para motores DRV8825 y L298N.
* `src/dispositivos/servo/ServoBoli.cpp`: Control del servo para el bolígrafo.
* `src/include/config.hpp`: Configuración de pines y parámetros.
* `src/io/FifoReader.cpp`: Lectura de comandos y archivos G-code desde FIFO.

## Comandos G-code soportados

- `G1 Xn Yn` — Mueve el cabezal a la posición (X, Y).
- `M1` — Levanta el bolígrafo.
- `M2` — Baja el bolígrafo.

## Créditos

Desarrollado por David Gerardo Martínez Armero.

---

¡Contribuciones y mejoras son bienvenidas!




## Nuevas funcionalidades

Desde la nueva versión, el programa ahora puede:

- **Escuchar comandos G-code en tiempo real** a través de un FIFO (`/tmp/gcode_pipe`).
- **Aceptar archivos `.gcode` durante la ejecución** escribiendo una línea con `@ruta/del/archivo.gcode` en el FIFO.

Esto permite enviar comandos desde otros programas, scripts o una interfaz web sin reiniciar el programa.









