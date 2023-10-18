// n11046244

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

int main(void)
{
    cli();
    button_init();
    pwm_init();
    adc_init();
    // uart_init();
    spi_init();
    timer_init();
    sei();

    // main loop
    while (1)
    {
        switch (mode)
        {
        case SEQUENCE_SELECT_MODE:
            SequenceSelectMode();
            break;
        case SEQUENCING_MODE:
            break;
        case TEST_MODE:
            break;
        default:
            mode = SEQUENCE_SELECT_MODE;
            break;
        }
    }
    return 0;
}
