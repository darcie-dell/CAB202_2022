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

//switch to set what state of sequencing we are in 
void SeqSTATE(void)
{
        switch (seqSTATE)
        {
        case START: //calls sequence initalization

            StartOfaSequence(); 

            break;
        case SEQUENCING:

            break;
        case END:
            break;
        default:
            mode = START;
            break;
        }
    
}
