#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

void random_init(uint16_t seed);
uint8_t random(void);

#endif // RANDOM_H
