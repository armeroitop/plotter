#pragma once
//#include "../dispositivos/motores/DRV8825Driver.hpp"
//#include "../dispositivos/interruptores/FinalDeCarrera.hpp"
#include <string>
#include <utility> //std::pair
#include <chrono>
#include <deque>
#include <optional>

// Forward declarations
class MotorDriver;
class FinalDeCarrera;
class Parametros;

struct PlanificadorDeMovimiento {
    MotorDriver* p_motorX = nullptr;
    MotorDriver* p_motorY = nullptr;

    FinalDeCarrera* p_finXmin = nullptr;
    FinalDeCarrera* p_finXmax = nullptr;
    FinalDeCarrera* p_finYmin = nullptr;
    FinalDeCarrera* p_finYmax = nullptr;

    Parametros& parametros;

    std::string ultimoFinDeCarreraActivado;

    float x_actual = 0, y_actual = 0;
    float x_ultimo = 0, y_ultimo = 0;

    float velocidadUnitariaMax = 0;
    float velocidadX = 0;
    float velocidadY = 0;
    int aceleracion = 500; // pasos por segundo al cuadrado. Con (a=500) esto aceleraremos a tope en 10 pasos

    float velocidadCoef = 0.0f; // empieza desde 0 (parado)
    float coefStep = 0.0f;
    int pasosParada; // pasos que dará acelerando
    std::chrono::steady_clock::time_point t_anterior;

    int sentidoMX_actual = 0;
    int sentidoMY_actual = 0;
    int sentidoMX_ultimo = 0;
    int sentidoMY_ultimo = 0;
    int sentidoMX_siguiente = 0;
    int sentidoMY_siguiente = 0;

    bool debeFrenar = true;
    bool debeAcelerar = true;


    /*Recorre un paso cada 10000 microsegundos que será entonces 1/10000 = 0.0001*/
    float velocidadAngularMax = 0.0001f; // pasos/microisegundos

    int  velocidadPasosPorSegundo = 100; // salen a 100 pasos/segundo

    bool paradaEmergencia = false;

    explicit PlanificadorDeMovimiento();

    void setMotores(MotorDriver& motorX, MotorDriver& motorY);

    void setFinalesDeCarrera(FinalDeCarrera& finXmin, FinalDeCarrera& finXmax,
                             FinalDeCarrera& finYmin, FinalDeCarrera& finYmax);

    void setVelocidadMaxima(float velocidad);
    void setAceleracionMaxima(float aceleracion);

    void acelerarTiemposDePaso(int64_t& tiempoPasoX, int64_t& tiempoPasoY);

    // Define cuántos pasos por unidad (e.g., mm o grados).
    void setResolucionPaso(float pasosPorUnidad);

    // Calcula trayectorias y activa los motores.
    void moverA(float x, float y, const std::optional<std::pair<float, float>>& siguienteG1);

    // Mueve de forma relativa a la posición actual.
    void moverRelativo(float deltaX, float deltaY, const std::optional<std::pair<float, float>>& siguienteG1);

    void moverMotorX(int pasos, bool direccion); // Avanza pasos en X.
    void moverMotorY(int pasos, bool direccion); // Avanza pasos en Y.

    /**
     * @brief Calcula la cantidad de pasos que debe dar desde la posición
     * actual a los incrementos de posición en los ejes x e y
     * 
     * @param deltaX 
     * @param deltaY 
     * @param pasosMotorX 
     * @param pasosMotorY 
     */
    void calcularPasos(float deltaX, float deltaY,
                        int& pasosMotorX,
                        int& pasosMotorY);

    /**
     * @brief Adelanta un Calculo de la cantidad de pasos necesarios para el 
     * paso siguiente
     * 
     * @param deltaX 
     * @param deltaY 
     * @param posicionSiguienteX 
     * @param posicionSiguienteY 
     * @param pasosMotorX 
     * @param pasosMotorY 
     */
    void calcularPasos(float x, float y,
        float posicionSiguienteX, float posicionSiguienteY,
        int& pasosMotorX,
        int& pasosMotorY);


    void calcularTiemposDePaso(const float deltaX, const float deltaY,
        int64_t& tiempoPasoX,
        int64_t& tiempoPasoY);

    int calcularPasosRestantes();

    void guardarUltimaPosicion();

    void calcularPosicionActual(int pasosMotorX, int pasosMotorY, int sentidoMX, int sentidoMY);

    void rotar();    // Realiza un paso en la trayectoria.
    void detenerSiCompletado();
    void detener();  // Detiene todos los motores.
    void pausar();   // Pausa el movimiento actual.
    void arrancar(); // Reanuda después de una pausa.

    bool movimientoEnCurso(); // Devuelve si hay un movimiento en proceso.
    bool alcanzaFinalDeCarrera();    // Devuelve si se ha alcanzado un final de carrera.
    void actualizarPosicionPorPisarFinalDeCarrera();
    bool comprobarFin(FinalDeCarrera* sensor, const std::string& nombre);
    std::pair<float, float> obtenerPosicion(); // Retorna la posición actual.
    void actualizarPosicion(float x, float y); // Actualiza la posición actual.

    // Enviar la poscion x y por fifowriter
    void enviarPosicionFifo();

    void configurarMotores(int pasosMotorX, int pasosMotorY, int64_t tiempoPasoX, int64_t tiempoPasoY);

    /**
     * @brief Si hay cambio de dirección respecto del paso anterior, aceleramos 
     * en la arrancada del nuevo paso
     * 
     * @return true 
     * @return false 
     */
    bool get_debeAcelerar();


    // Permite suavizar el arranque y la detención utilizando aceleración y
    // deceleración.
    float calcularVelocidadInterpolada(float distancia, float velocidadMaxima);

    // Configura la posición actual como (0,0).
    void establecerOrigen();

    // Detecta los límites mecánicos y los configura.
    void calibrar();

    void activarParadaDeEmergencia();
    void desactivarParadaDeEmergencia();
    bool esParadaDeEmergencia() const; // Método para verificar el estado de la parada de emergencia

    // Envia informacion general por fifo
    void enviarInformacionGeneralFifo();
};
