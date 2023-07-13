//#define PORTA *((volatile unsigned char*) 0x3B)
//#define DDRA *((volatile unsigned char*) 0x3A)
//#define PA7 7
#ifndef __AVRIO__
#define __AVRIO__
#include <avr/io.h>
#endif

#ifndef _RANDOM_H_
#include "random.h"
#endif

void delay(volatile long time) {
    while (time > 0) {
        time--;
    }
}

int main() {
    //Set LED pins as output
    DDRA = (1 << PA7 | 1 << PA6 | 1 << PA5 | 1 << PA4);
    //Set analog read in (To seed the random function) as input
    DDRA &= ~(1 << PA1);

    //Two options: 
    //1. Measure the ADC, use that to seed
    //2. Time when the human interacts with the first button: use the lower levels of the timer

    PORTA = (1<<PA7 | 1 << PA6 | 1 << PA5 | 1 << PA4);

    random_init(0xabcd);

    while (1) {
        //Turns all LEDs off
        PORTA = 0x00;
        delay(100000);
        //Allocates space in memory for this 8 bit variable
        //This will store our random number
        uint8_t randomValue = random() % 4;
        //Turns on one of the LEDs at random
        PORTA = (0b000010000 << randomValue);
        //delay(10000*randomValue);
        //PORTA = 0xFF;
        delay(100000);
    }
}