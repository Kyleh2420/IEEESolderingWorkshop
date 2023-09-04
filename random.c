#include "random.h"
#include <avr/io.h>

static uint16_t random_seed;

void random_init(uint16_t seed) {
    random_seed = seed;
}

uint8_t random(void) {
    // Pseudo-random number generation using a simple linear congruential generator (LCG)
    random_seed = (random_seed * 32719 + 3) % 32749;
    
    // Limit the range to 0-3 (modulo 4)
    return random_seed % 4;
}