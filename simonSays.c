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
    uint8_t test = 0b00010000;
    for (int i = 0; i < currentIndex; i++) {
        //Wait for button release
        while (inp != 0x00) {
            inp = readButton();
        }
        while (inp == 0x00) {
            //keep updating newButt until they are different
            inp = readButton();
        }

        //Play the button back for user feedback
        PORTA = inp;
        delay(50000);
        PORTA = 0x00;
        delay(50000);

        //Once we escape the polling loop, then something has changed.
        //If the given input is the same as the sequence, then it is correct
        //Otherwise, it is wrong
        test = 0b00010000;
        test = test << sequence[i];

        if (inp != test) {
            //If the input does not equal the test, then the user has failed.
            //Blink the correct LED, and return 1.
            // for (int j = 0; j < 5; j++) {
            //     PORTA = test;
            //     delay(50000);
            //     PORTA = 0x00;
            //     delay(50000);
            // }
            return 1;
        }
        //If the input does equal the test, then the user passed. Simply move on.          
    }
    //If we escape from the for loop, then we have exhausted the current index. Return 0, the user is all good.
    return 0;
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
    uint8_t index = 0;

    while (1) {
        if (playing) {
            //currentSeq[index] = random() % 4;
            currentSeq[index] = index % 4;
            //If the user exceeds the max length, they have won the game
            if (index >= MAXLEN) {
                playing = false;
                failed = false;
            } else {
                playSequence(index, currentSeq);
                if (checkUserInput(index, currentSeq) == 1) {
                    playing = false;
                    failed = true;
                }
                index++;
            }
            
            delay(50000);

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
                
    }
}