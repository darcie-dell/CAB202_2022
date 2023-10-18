#include <stdint.h>
#include <stdio.h>

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


// initialise sequence index to 0
volatile uint8_t sequence_index = 0;

// initialise mode to sequence select mode
volatile program_mode mode = SEQUENCE_SELECT_MODE;

// initialise which side to display for the Sequence Select display
volatile char DispSide = 'L';

// initialise current pushbutton state
volatile uint8_t pb_state = 0xFF;

// initialise previous pushbutton state
volatile uint8_t pb_prev = 0xFF;

//initalise offset to 0
volatile uint32_t offset = 0;

//initalise sequence state to start 
volatile SEQUENCE_STATE seqSTATE = START;

// Initialise the pulse width modulation for brightness and frequency.
void pwm_init()
{
    PORTMUX.TCAROUTEA = PORTMUX_TCA02_ALT1_gc;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV2_gc;
    //TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm;

    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP0EN_bm;

    //set inital values (random)
    TCA0.SINGLE.PER = 10000;
    TCA0.SINGLE.CMP0 = 10000 >> 1;
    TCA0.SINGLE.CMP1 = 10000 >> 1;

    // ENABLE SO WE DONT CLOBBER CLOCK SLECTION
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

// initialise the analogue to digital converter
void adc_init(void)
{
    // enable ADC
    ADC0.CTRLA = ADC_ENABLE_bm;
    // prescaler lowest is 2
    ADC0.CTRLB = ADC_PRESC_DIV2_gc;
    // set control port C
    ADC0.CTRLC = (4 << ADC_TIMEBASE_gp) | ADC_REFSEL_VDD_gc;
    // Sample duration of 64
    ADC0.CTRLE = 64;
    // Free running, left adjust result
    ADC0.CTRLF = ADC_FREERUN_bm;
    // Select AIN2 (potentiomenter R1)
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
    // Select 12-bit resolution, single-ended
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc | ADC_START_IMMEDIATE_gc;
}

// initalise TCB1 for duration to * by 13.1
void timer_init(void)
{
    TCB1.CTRLB = TCB_CNTMODE_INT_gc;
    TCB1.CCMP = 21845;
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_DIV2_gc;;
}

//TCB interrupt for pb sampling 
ISR(TCB0_INT_vect)
{
    static uint8_t count0 = 0;
    static uint8_t count1 = 0;

    if (mode == SEQUENCE_SELECT_MODE)
    {
        // get sample of button
        uint8_t pb_sample = PORTA.IN;

        // see if there was a chnage
        uint8_t pb_changed = pb_sample ^ pb_state;

        // increment count by one ifstate changed, reset otherwise
        count1 = (count1 ^ count0) & pb_changed;

        count0 = ~count0 & pb_changed;

        // update PB state immediately on falling edge or if PB high for three samples
        pb_state ^= (count1 & count0) | (pb_changed & pb_state);

        // display sequence index
        SequenceSelectDisplay(sequence_index);
    }
    else if (mode == SEQUENCING_MODE)
    {
        // display 8 on lhs
        SequenceDisplay();
    }

    // Acknowledge interrupt
    TCB0.INTFLAGS = TCB_CAPT_bm;
}

// duration interrupt
ISR(TCB1_INT_vect)
{
    //if we are in sequencing mode
    if (mode == SEQUENCING_MODE)
    {
        //and we are sequencing
        if (seqSTATE == SEQUENCING)
        {
            // and if duration is up get and set the new sequence values 
            if (CurrentDuration == 0)
            {
                GetNextStep();
                SetNextStep();
            }

            //if duration is not 0 decrease counter until it is and get next step
            CurrentDuration--;
        }
    }

        // Acknowledge interrupt
    TCB1.INTFLAGS = TCB_CAPT_bm;
}


void button_init(void)
{
    // enable pullup resistors on pushbuttons
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;

    // initial timer for pb smapling
    TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Configure TCB0 in periodic interrupt mode
    TCB0.CCMP = 33333;               // Set interval for 1ms (3333 clocks @ 3.3 MHz)
    TCB0.INTCTRL = TCB_CAPT_bm;      // CAPT interrupt enable
    TCB0.CTRLA = TCB_ENABLE_bm;      // Enable
}



