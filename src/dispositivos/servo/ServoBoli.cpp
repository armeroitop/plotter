#include "ServoBoli.hpp"
#include "fifo/FifoWriter.hpp"



ServoBoli::ServoBoli(int pin, int anguloLevantado, int anguloBajado) :
    pin(pin), anguloLevantado(anguloLevantado), anguloBajado(anguloBajado), esAbajo(false) {

    //pinMode(pin, PWM_OUTPUT);
    //pwmSetRange(1000);

    // Configuración del pin para PWM por software
    if (wiringPiSetup() == -1) {
        std::cerr << "Error al configurar WiringPi!" << std::endl;
        exit(1);
    }

    softPwmCreate(pin, 0, 100);
    //printf("softPwmCreate devolvió: %d\n", res);
}

ServoBoli::~ServoBoli() {
    // Detener PWM al destruir el objeto
    softPwmStop(pin);
    printf("Destructor llamado: ServoBoli detenido.\n");
}

void ServoBoli::levantar() {
    if (!esAbajo) return;

    moverServo(3); // 3 es el valor mínimo que acepta el servo
    esAbajo = false;
    printf("levanta\n");

    FifoWriter::write("[ServoBoli] metodo: M1"); // FIXME: habría que pensar mejor esto
   
    delay(300);
}

void ServoBoli::bajar() {
    if (esAbajo) return;

    moverServo(23); // 23 es el valor maximo que acepta el servo
    esAbajo = true;
    printf("baja\n");

    FifoWriter::write("[ServoBoli] metodo: M2");// FIXME: habría que pensar mejor esto

    delay(300);
}

bool ServoBoli::estaAbajo() const {
    return esAbajo;
}

void ServoBoli::liberar() {
    // Liberar tensión en el servo
    softPwmWrite(pin, 0); // deja de enviar PWM, servo queda flojo    
    printf("Liberando tensión en el servo.\n");
}

void ServoBoli::moverServo(int angulo) {
    printf("mover: %d\n", angulo);
    softPwmWrite(pin, angulo);
}
