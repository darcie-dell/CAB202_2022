// modes

#include <stdio.h>
#include <stdint.h>

#include "sequence.h"

#include "decoder.h"
#include "uart.h"
#include "spi.h"
#include "initialisation.h"
#include "modes.h"
#include "Start.h"
#include "calcs.h"
#include "states.h"
#include "seqStates.h"

//decodeas, descrables and sperates out the sequence 
void GetNextStep(void)
{

    //reset registers 
    PORTB.DIRSET = PIN1_bm;
    PORTB.DIRSET = PIN0_bm;

    //change state to sequencing 


    uint8_t achar = pgm_read_byte_near(SEQUENCE + offset);

    uint8_t bchar = pgm_read_byte_near(SEQUENCE + offset + 1);

    uint8_t cchar = pgm_read_byte_near(SEQUENCE + offset + 2);

    uint8_t dchar = pgm_read_byte_near(SEQUENCE + offset + 3);

    offset = offset + 4;

    uint32_t decoded = decode2(achar, bchar, cchar, dchar);

    NextDuration = decoded >> 16;

    NextBrightness = decoded >> 8;
    uint8_t freqdata = decoded;

    descramble2(&NextDuration);
    descramble2(&NextBrightness);
    descramble2(&freqdata);

    NextOctave = freqdata >> 4;

    NextNote = freqdata & 0b00001111;
}

void SetNextStep(void)
{
    seqSTATE = SEQUENCING;

    if (NextDuration == 0)
    {
        // stop all sound and light 
        PORTB.DIRCLR = PIN0_bm;
        PORTB.DIRCLR = PIN1_bm;

        //go back to sequence select mode for next selection 
        mode = SEQUENCE_SELECT_MODE;
    }

    else
    {

        // reset duration for next step
        CurrentDuration = NextDuration;

        uint32_t Freq;

        Freq = FrequencyLookup[NextNote] >> NextOctave;

        TCA0.SINGLE.PER = Freq;

        TCA0.SINGLE.CMP0BUF = (Freq >> 1);

        if (NextOctave == 0){
            //turn buzzer off when octave is 0
            PORTB.DIRCLR = PIN0_bm;
        }


        if (NextBrightness == 0)
        {
            TCA0.SINGLE.CMP1BUF = 0;
        }

        else
        {
            TCA0.SINGLE.CMP1BUF = (Freq * (NextBrightness + 1)) >> 8;
        }
    }
}

void GetPushButtonState(void)
{
    // initalise states
    uint8_t captured_state = pb_state;
    // or to see if has chnaged
    uint8_t pb_changed = captured_state ^ pb_prev;
    // and to see if the change was falling
    uint8_t pb_falling = pb_changed & pb_prev;
    // storing previous state as a varible
    pb_prev = captured_state;

    if ((captured_state & PIN4_bm) == 0)
    {
        // if s1 pressed get the potenometer position
        sequence_index = ADC0.RESULT;
    }
    else if (pb_falling & PIN5_bm)
    {
        // if there is a falling edge on S2
        if (sequence_index > 0)
        {
            // decrement sequence index by 1
            --sequence_index;
        }
    }
    else if (pb_falling & PIN6_bm)
    {
        // if theres a falling edge on S3
        if (sequence_index < 255)
        {
            // increment index by 1
            ++sequence_index;
        }
    }
    else if (pb_falling & PIN7_bm)
    {
        PORTB.OUTSET |= PIN0_bm;

        PORTB.DIRSET |= PIN0_bm;

        PORTB.OUT ^= PIN0_bm;

        seqSTATE = START;

        mode = SEQUENCING_MODE;

        CurrentDuration = 0;

        SeqSTATE();

        // TCA0.SINGLE.PER = 10000;

        // PORTB.DIRSET = PIN1_bm;

        // TCA0.SINGLE.CMP0 = 0;
        // TCA0.SINGLE.CMP1 = 10000 >> 1;
    }
}

// sequence-slect_mode just takes push button state
void SequenceSelectMode(void)
{
    while (mode == SEQUENCE_SELECT_MODE)
    {
        GetPushButtonState();
    }
}

void StartOfaSequence(void)
{
    state = 0x11046244;

    offset = sequence_index * 32;

    for (uint8_t i = 0; i < sequence_index * 24; i++)
    {
        next();
    }

    seqSTATE = SEQUENCING;

    SeqSTATE();

    // SeqSTATE();
}

void NextSequenceDetected(void)
{

    mode = SEQUENCE_SELECT_MODE;

    seqSTATE = START;

    SeqSTATE();
}