#include <stdint.h>
#include <avr/interrupt.h>

#include "states.h"

#include "seqStates.h"



void pwm_init(void);
void adc_init(void);

void timer_init(void);
ISR(TCB0_INT_vect);

void button_init(void);

volatile uint8_t sequence_index;
// initialise mode to sequence select mode
volatile program_mode mode;
// initialise which side to display for the Seqence Select display
volatile char DispSide;
// initialise current pushbutton state
volatile uint8_t pb_state;
// initialise previous pushbutton state
volatile uint8_t pb_prev;
volatile uint32_t offset;
volatile uint8_t CheckIfStart;

volatile SEQUENCE_STATE seqSTATE;