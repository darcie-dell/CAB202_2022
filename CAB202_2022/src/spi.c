#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "initialisation.h"

#include "decoder.h"
#include "uart.h"
#include "spi.h"
#include "initialisation.h"
#include "modes.h"
#include "Start.h"
#include "calcs.h"
#include "states.h"
#include "seqStates.h"


volatile uint8_t data;


uint8_t sevenSegHex[] = {
    0b00001000, 0b01101011, 0b01000100, 0b01000001,
    0b00100011, 0b00010001, 0b00010000, 0b01001011,
    0b00000000, 0b00000001, 0b00000010, 0b00110000,
    0b00011100, 0b01100000, 0b00010100, 0b00010110,
};

//Initialise SPI 
void spi_init(void) {
    //enable DISP EN as output
    PORTB.DIRSET |= PIN1_bm;
    PORTB.OUTSET |= PIN1_bm;
    //enable DISP LATCH as output
    PORTA.DIRSET = PIN1_bm; //determines if it's an input/output
    PORTA.OUTSET = PIN1_bm; //determines the actual output
    
    PORTC.DIR |= (PIN0_bm | PIN2_bm);
    
    PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc;
    //SPI0 configuration
    SPI0.CTRLA = SPI_MASTER_bm;
    SPI0.CTRLB = SPI_SSD_bm;
    //enable interrupts
    SPI0.INTCTRL = SPI_IE_bm;
    //enable spi
    SPI0.CTRLA |= SPI_ENABLE_bm;
}

//write data to display
void spi_write(uint8_t b) {
    SPI0.DATA = b;
}

ISR(SPI0_INT_vect) {
    //set to low then set to high to create a rising edge
    PORTA.OUTCLR = PIN1_bm; //clear pin1 
    PORTA.OUTSET = PIN1_bm; //set pin1 


    data = SPI0.DATA;

    SPI0_INTFLAGS |= (1<<7);
}

//get hex of nibble either ms or ls by passing number and the most or least sig nibble 
uint8_t GetHex(uint8_t num, char nibble) {
    if (nibble == 'L') {
        uint8_t LSNibble = (num) & 0b00001111;
        //return hex of the value 
        return sevenSegHex[LSNibble];
    } else {
        uint8_t MSNibble = (num) >> 4;
        //get hex of the ms nibble 
        return sevenSegHex[MSNibble] | 0b10000000;
    }
}

// displays the sequence index, as num 
void SequenceSelectDisplay(uint8_t num) {
    //display selected index as hexadecimal - iterate left andd right
    if (DispSide == 'L') {
        //MS nibble 
        spi_write(GetHex(num, 'M'));
        //chnage side 
        DispSide = 'R';
    } else {
        //LS nibble
        spi_write(GetHex(num, 'L'));
        //chnage side 
        DispSide = 'L';
    }
}

void SequenceDisplay(void) {
    //display an 8 on the left side of the display, nothing on the right during sequencing
    spi_write(sevenSegHex[8] | 0b10000000);
}