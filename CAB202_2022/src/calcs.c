#include <stdint.h>
#include <stdio.h>

#include "decoder.h"
#include "uart.h"
#include "spi.h"
#include "initialisation.h"
#include "modes.h"
#include "Start.h"
#include "calcs.h"
#include "states.h"
#include "seqStates.h"

volatile uint8_t NextDuration;
volatile uint8_t currentDuration;
volatile uint8_t NextBrightness;
volatile uint8_t NextOctave;
volatile uint8_t NextNote;



//frequencies on my 3.33 /DIV 2 clock 
uint32_t FrequencyLookup[12] = {60606, 57204, 53994, 50963, 48103, 45403, 42855, 40450, 38179, 36037, 34014, 32105};


uint32_t BrightnessCalc(uint8_t brightnessValue, uint32_t freq)
{
    uint32_t DutyCycle = (freq * (brightnessValue + 1)) >> 8;
    return DutyCycle;
}

uint32_t ToneCalc(uint8_t note, uint8_t octave)
{
    if (octave == 0)
    {
        return 0;
    }
    else
    {
        uint32_t freq = FrequencyLookup[note];
        freq = freq >> octave;
        return freq;
    }
}
