#pragma once
//#include "../dispositivos/motores/DRV8825Driver.hpp"
//#include "../dispositivos/interruptores/FinalDeCarrera.hpp"
#include <string>
#include <utility> //std::pair

// Forward declarations
class MotorDriver;
class FinalDeCarrera;

struct PlanificadorDeMovimiento {
    MotorDriver* p_motorX = nullptr;
    MotorDriver* p_motorY = nullptr;

    FinalDeCarrera* p_finXmin = nullptr;
    FinalDeCarrera* p_finXmax = nullptr;
    FinalDeCarrera* p_finYmin = nullptr;
    FinalDeCarrera* p_finYmax = nullptr;

    std::string ultimoFinDeCarreraActivado;

    float x_actual = 0, y_actual = 0;
    float x_ultimo = 0, y_ultimo = 0;

    float velocidadUnitariaMax = 0;
    float velocidadX = 0;
    float velocidadY = 0;
    float aceleracion = 0;

    float velocidadCoef = 0.0f; // empieza desde 0 (parado)
    float coefStep = 0.0f;


    /*Recorre un paso cada 10000 microsegundos que será entonces 1/10000 = 0.0001*/
    float velocidadAngularMax = 0.0001f; // pasos/microisegundos

    int  velocidadPasosPorSegundo = 100// salen a 100 pasos/segundo

    bool paradaEmergencia = false;

    explicit PlanificadorDeMovimiento();

    void setMotores(MotorDriver& motorX, MotorDriver& motorY);

    void setFinalesDeCarrera(FinalDeCarrera& finXmin, FinalDeCarrera& finXmax,
                             FinalDeCarrera& finYmin, FinalDeCarrera& finYmax);

    void setVelocidadMaxima(float velocidad);
    void setAceleracionMaxima(float aceleracion);

    void acelerarTiemposDePaso(int& tiempoPasoX, int& tiempoPasoY);

    // Define cuántos pasos por unidad (e.g., mm o grados).
    void setResolucionPaso(float pasosPorUnidad);

    // Calcula trayectorias y activa los motores.
    void moverA(float x, float y);

    // Mueve de forma relativa a la posición actual.
    void moverRelativo(float deltaX, float deltaY);

    void moverMotorX(int pasos, bool direccion); // Avanza pasos en X.
    void moverMotorY(int pasos, bool direccion); // Avanza pasos en Y.

    void calcularPasos(float deltaX, float deltaY,
                        int& pasosMotorX,
                        int& pasosMotorY);

    void calcularTiemposDePaso(const float deltaX, const float deltaY,
        int& tiempoPasoX,
        int& tiempoPasoY);

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

    void configurarMotores(int pasosMotorX, int pasosMotorY, int tiempoPasoX, int tiempoPasoY);


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
