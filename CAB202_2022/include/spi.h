#include <stdint.h>
#include <avr/interrupt.h>

void spi_init(void);
void spi_write(uint8_t);
ISR(SPI0_INT_vect);
uint8_t GetHex(uint8_t num, char nibble);
void SequenceSelectDisplay(uint8_t);
void SequenceDisplay(void);