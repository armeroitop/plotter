#include <stdio.h>
#include <wiringPi.h>

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.

#define	LED	16

int main(void) {
    printf("Raspberry Pi blink\n");

    wiringPiSetup();
    pinMode(LED, OUTPUT);



    for (int i = 0;i < 5;i++) {
        printf("Vuelta numero %d \n", i);
        digitalWrite(LED, HIGH);	// On
        delay(500);		// mS
        digitalWrite(LED, LOW);	// Off
        delay(500);
    }
    return 0;
}