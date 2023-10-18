#include <stdint.h>

void GetNextStep(void);
void GetNextStep2(void);
void GetPushButtonState(void);
void SequenceSelectMode(void);
void SequencingMode(void);
void TestMode(void);
void SetNextStep(void);
void StartOfaSequence(void);
void NextSequenceDetected(void);

volatile uint8_t NextDuration ;
volatile uint8_t NextBrightness;
volatile uint8_t NextOctave;
volatile uint8_t NextNote;
volatile uint8_t CurrentDurationLeft;
volatile uint8_t CurrentDuration;