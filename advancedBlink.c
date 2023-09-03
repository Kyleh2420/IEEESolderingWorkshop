
#ifndef __AVRIO__
#define __AVRIO__
#include <avr/io.h>
#endif

#ifndef _RANDOM_H_
#include "random.h"
#endif

#include <stdbool.h>

#define MAXLEN 5


//returns the value of the buttons (In PORTB, left justified)
uint8_t readButton () {
	uint8_t buttons = (PINB & 0b01111000) << 1;
    buttons = ~buttons;
    buttons = buttons & 0xF0;
	return buttons;
}

void delay(volatile long time) {
	while (time > 0) {
		time--;
	}
}

void ADC_init() {
    //Sets the refernce voltage to AVCC
    ADMUX |= (1 << REFS0);

    //Enable ADC and set prescaler to 128
    ADCSRA |= (1 << ADEN) |
                (1 << ADPS2) |
                (1 << ADPS1) | 
                (1 << ADPS0);
}

// Function to read analog value from ADC
uint16_t ADC_read(uint8_t channel) {
    // Clear the channel selection bits
    ADMUX &= 0xF0;
    // Set the channel selection bits based on the given channel
    ADMUX |= (channel & 0x0F);
    
    // Start ADC conversion
    ADCSRA |= (1 << ADSC);
    
    // Wait until the conversion is complete
    while (ADCSRA & (1 << ADSC));
    
    // Return the ADC value
    return ADC;
}

int setup() {

/*
    This setup function will perform the following tasks:


    [Setup IO ports]
    Set the LED pins as pinout (PA7-PA4)
    Set the pushbutton pins as input (PB6-PB3)
    Set the seed for our RNG as input (PB1)

    [Initialize the ADC]
    Set the proper registers for ADC to function properly
    Seed the RNG function with something form the ADC

    [Power On Self Test (POST)]
    Turn on all LEDs for 1 second, then off

*/


    //Set LED pins as output
    DDRA = (1 << PA7)|(1 << PA6)| (1 << PA5)| (1 << PA4);

    //Set the Pushbutton Pins as Input
    DDRB &= ~((1 << PB6) | (1 << PB5) | (1 << PB4) | (1 << PB3));

    //Sets PA1 as input
    DDRA &= ~(1 << PA1);

    //Set all LEDs as high
    PORTA = (1 << PA7) | (1 << PA6) |  (1 << PA5) | (1 << PA4);

    //Set analog read in (To seed the random function) as input
    DDRA &= ~(1 << PA1);

    ADC_init();
    random_init(ADC_read(1));

	PORTA = 0xF0;
	delay(100000);
	PORTA = 0x00;

    
}

/*
    Outputs the sequence up until the currentIndex to the LEDs using a read mod write method
*/

void playSequence(int currentIndex, uint8_t sequence [ MAXLEN ]) {

    // for (int i = 0; i < 10; i++) {
    //             PORTA = sequence[0];
    //             delay(100000);
    //             PORTA = sequence[1];
    //             delay(100000);
    // }


    for (int i = 0; i < currentIndex; i++) {
        //uint8_t currentA = PORTA;
        uint8_t currentA = sequence[i];
        currentA &= 0b00001111;
        currentA |= 0b00010000 << sequence[i];
        PORTA = currentA;
        delay(105000);
        PORTA = 0x00;
        delay(105000);
    }
}

/*
    Loads the array from the very beginning, then proceeds with the checking algorithm.
    Does nothing until user presses a button. Checks agianst that index of the given sequence.
    If correct, moves on. 
    If incorrect, plays "Wrong input"
*/
uint8_t checkUserInput(int currentIndex, uint8_t sequence [ MAXLEN ]) {
    // Declare a boolean variable that checks to see if the items are the same
    // If they are not the same, then something has changed. Move on
    // If they are the same, keep checking.

    // Check to see if the input matches the current index in sequence. If it does, keep going.
    // If it does not match, then something is wrong. Exit with status 1.

    // When the entire sequence up until the current index is exhausted, then return 0.
    uint8_t inp = 0xF0;

    for (int i = 0; i < currentIndex; i++) {
        //Wait for button release
        while (inp != 0x00) {
            inp = readButton();
        }
        while (inp == 0x00) {
            //keep updating newButt until they are different
            inp = readButton();
        }
        //Once we escape the polling loop, then something has changed.

        for (int j = 0; j < 5; j++) {
            //Blink to acknowlege input
            PORTA = 0b10010000;
            delay(10000);
            PORTA = 0x00;
            delay(10000);
        }
        
    }
}

/*
    [Sets up the mcu]
    Calls setup function

    [Declares global variables]
    Sets space in memory for the array
    Declares the index number
    
*/
int main() {
    setup();
    uint8_t currentSeq [ MAXLEN ];
    bool playing = true;
    bool failed = false;
    // currentSeq[0] = 0b10000000;
    // currentSeq[1] = 0b01000000;
    uint8_t index = 0;
	while (1) {
        if (playing) {
            currentSeq[index] = random() % 4;

            //If the user exceeds the max length, they have won the game
            if (index >= MAXLEN) {
                playing = false;
            } else {
                playSequence(index, currentSeq);
                checkUserInput(index, currentSeq);

                index++;
            }
            
            delay(10000);

            PORTA = 0xF0;
            delay(100000);
            PORTA = 0x00;
            delay(100000);

        //If the user has stopped playing, check to see if they've failed or they've passed the game
        } else {
            if (failed) {
                PORTA = 0b01000000;
                delay(10000);
                PORTA = 0x00;
                delay(10000);
            } else {
                PORTA = 0b00100000;
                delay(10000);
                PORTA = 0x00;
                delay(10000);
            }
        }
        


        //uint8_t randomValue = (0b00010000 << random() % 4);

        // uint8_t randomValue = 0b00010000;
        // //Turns on one of the LEDs at random
        // PORTA = randomValue;
        // delay(100000);
        // PORTA = 0x00;     
        // uint8_t comparison = readButton() == randomValue;   

        // if (comparison) {
        //     for (int i = 0; i < 10; i++) {
        //         PORTA = 0b00100000;
        //         delay(10000);
        //         PORTA = 0x00;
        //         delay(10000);
        //     }
        // } else {
        //     for (int i = 0; i < 10; i++) {
        //         PORTA = 0b01000000;
        //         delay(10000);
        //         PORTA = 0x00;
        //         delay(10000);
        //     }
        // }
		//intake Port B's values and mask it with 0b0111_1000, then left shift 1 times.
		//PORTA = readButton();
		
		
	}
}


