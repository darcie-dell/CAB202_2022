#include <stdint.h>

volatile uint8_t NextDuration;
volatile uint8_t currentDuration;
volatile uint8_t NextBrightness;
volatile uint8_t NextOctave;
volatile uint8_t NextNote;

uint32_t FrequencyLookup[12];

uint32_t BrightnessCalc(uint8_t brightnessValue, uint32_t freq);

uint32_t ToneCalc(uint8_t note, uint8_t octave);