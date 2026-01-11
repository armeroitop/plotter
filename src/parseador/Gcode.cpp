#include "Gcode.hpp"
#include <sstream>
#include <iostream>

Gcode::Gcode(PlanificadorDeMovimiento& planificador)
    : planificador(planificador) { 

        // TODO: El Gcode no debería conocer a servoBoli. Este se debería de mover al planificador
        // y que toda la lógica se haga allí
    }


/**
 * @brief  Interpreta una instrucción G-code y ejecuta la acción correspondiente.
 *
 * @param instruccion
 */
void Gcode::interpretar(const std::string& instruccion, std::deque<std::string>& bufferMovimientos) {
    std::istringstream entrada(instruccion);
    std::string comando;
    entrada >> comando;

    std::cout << "[Gcode::interpretar] Recibido: " << instruccion << std::endl;

    if (comando == "G1" || comando == "G01" || comando == "G00") {
        double x = 0, y = 0, z = 0, f = 0;
        bool hasX = false, hasY = false, hasZ = false, hasF = false;
        char eje;
        while (entrada >> eje) {
            if (entrada.peek() == '\n') break;  // seguridad
            eje = toupper(eje);

            if (eje == 'X') {
                entrada >> x;
                hasX = true;
            } else if (eje == 'Y') {
                entrada >> y;
                hasY = true;
            } else if (eje == 'Z') {
                entrada >> z;
                hasZ = true;
            } else if (eje == 'F') {
                entrada >> f;
                hasF = true;
            } else {
                // ignorar cualquier otra letra
                float tmp; entrada >> tmp;
            }
        }

        // TODO: estaría bien trasladar esto al planificador
        if ( hasZ ){
            /*if (z > 0) servoBoli.levantar();
            else servoBoli.bajar(); */
            planificador.moverZ(z);            
        }

        std::optional<std::pair<float, float>> siguienteG1;
        if (bufferMovimientos.size() > 1) {
            // acceso por índice
            siguienteG1 = extraerCoordenadas(bufferMovimientos[1]);
        }

        if (hasX && hasY) {               
            if (modoRelativo) {
                printf("panificador.moverRelativo X: %f, Y: %f  \n", x, y);
                planificador.moverRelativo(x, y, siguienteG1);
                //planificador.moverRelativo(current.first, current.second, next);
            } else {
                printf("planificador.moverA X: %f, Y: %f  \n", x, y);
                planificador.moverA(x, y, siguienteG1);
                //planificador.moverA(current.first, current.second, next);
            }
        }

        // Eliminamos el comando ya ejecutado
        bufferMovimientos.clear();
    } else if (comando == "G02" || comando == "G03") {

        double x1 = 0, y1 = 0, I = 0, J = 0;
        bool sentidoHorario = (comando == "G02");
        char eje;
        while (entrada >> eje) {
            if (entrada.peek() == '\n') break;  // seguridad
            eje = toupper(eje);

            if (eje == 'X') {
                entrada >> x1;
            } else if (eje == 'Y') {
                entrada >> y1;
            } else if (eje == 'I') {
                entrada >> I;
            } else if (eje == 'J') {
                entrada >> J;
            } else {
                // ignorar cualquier otra letra
                float tmp; entrada >> tmp;
            }
        }

        std::optional<std::pair<float, float>> siguienteG1;
        if (bufferMovimientos.size() > 1) {
            // acceso por índice
            siguienteG1 = extraerCoordenadas(bufferMovimientos[1]);
        }

        planificador.moverArco(x1, y1, I, J, siguienteG1, sentidoHorario);

        // Eliminamos el comando ya ejecutado
        bufferMovimientos.clear();

    } else if (comando == "G90") {
        modoRelativo = false;
        std::cout << "[GCODE] Modo ABSOLUTO activado" << std::endl;

    } else if (comando == "G91") {
        modoRelativo = true;
        std::cout << "[GCODE] Modo RELATIVO activado" << std::endl;

    } else if (comando == "M1") {
        //servoBoli.levantar();
        std::cout << "[GCODE] Levantando el boli" << std::endl;

    } else if (comando == "M2") {
        //servoBoli.bajar();
        std::cout << "[GCODE] Bajando el boli" << std::endl;

    } else if (comando == "M100") { // peticion de informacion
        planificador.enviarInformacionGeneralFifo();

    } else if (comando == "M999") {
        planificador.desactivarParadaDeEmergencia();
        std::cout << "[GCODE] Liberar tensión en servo del boli y desactivar parada de Emergencia" << std::endl;

    } else if (comando == "M112") {
        planificador.activarParadaDeEmergencia();
        std::cout << "[GCODE] Parada de emergencia" << std::endl;

    } else {
        std::cerr << "Comando no reconocido: " << comando << std::endl;

    }
}


std::pair<float, float> Gcode::extraerCoordenadas(const std::string& comandoG1) {
    std::pair<float, float> coordenadas;
    std::istringstream entrada(comandoG1);
    std::string comando;
    entrada >> comando; // Leer "G1"

    char eje;
    while (entrada >> eje) {
        if (eje == 'X') {
            entrada >> coordenadas.first;
        } else if (eje == 'Y') {
            entrada >> coordenadas.second;
        }
    }

    return coordenadas;
}

