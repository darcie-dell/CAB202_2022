#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>


void next();

void descramble2(volatile uint8_t *a);


uint8_t indexInTable(const char a);

uint32_t decode2(const char a, const char b, const char c, const char d);

uint32_t state;

void advanceState(void);