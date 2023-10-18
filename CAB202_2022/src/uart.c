#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "decoder.h"
#include "uart.h"
#include "spi.h"
#include "initialisation.h"
#include "modes.h"
#include "Start.h"
#include "calcs.h"
#include "states.h"
#include "seqStates.h"


//Initialise UART to receive information via the terminal
void uart_init(void) {
    PORTB.DIRSET = PIN2_bm; // Enable PB2 as output (USART0 TXD)
    USART0.BAUD = 1389;     //  calculated buad
    USART0.BAUD = 4167; 
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // Enable Rx and Tx
    USART0.CTRLC = USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
}


//Get char
uint8_t uart_getc(void) {
    while (!(USART0.STATUS & USART_RXCIF_bm));  // Wait for data
    return USART0.RXDATAL;
}

//Put char
void uart_putc(uint8_t c) {
    while (!(USART0.STATUS & USART_DREIF_bm));  // Wait for TXDATA empty
    USART0.TXDATAL = c;
}

//Put string 
void uart_puts(char* string) {
    while (*(string) != '\0') {
        uart_putc(*string);
        ++string;
    }
    USART0.TXDATAL = '\0';
}