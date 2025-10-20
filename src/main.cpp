#include <Arduino.h>

#define BUZZER_PIN  27     // GPIO connected to buzzer
#define PWM_CH      0      // PWM channel (0–15)
#define FREQ        2000   // Default frequency (Hz)
#define RESOLUTION  10     // 10-bit resolution (0–1023)
// LEDs
#define LED1_PIN    18     // additional LED 1
#define LED2_PIN    19     // additional LED 2 (requested)

// State machine states for the repeating sequence
enum SequenceState {
  S_BEEPS,
  S_SWEEP,
  S_MELODY
};

// Timing/state variables
SequenceState state = S_BEEPS;
unsigned long stateStart = 0;
int sweepFreq = 400;
int beepIndex = 0;
int melodyIndex = 0;
const int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
const int melodyLen = sizeof(melody) / sizeof(melody[0]);

void setup() {
  // Setup PWM channel for buzzer
  ledcSetup(PWM_CH, FREQ, RESOLUTION);
  ledcAttachPin(BUZZER_PIN, PWM_CH);

  // Initialize LEDs
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  // initialize state
  state = S_BEEPS;
  stateStart = millis();
  beepIndex = 0;
  sweepFreq = 400;
  melodyIndex = 0;
}

void loop() {
  unsigned long now = millis();

  switch (state) {
    case S_BEEPS:
      // LED1 on during beeps, LED2 off
      digitalWrite(LED1_PIN, HIGH);
      digitalWrite(LED2_PIN, LOW);

      // each beep: tone 150ms on, 150ms off, 3 beeps
      if (beepIndex < 6) { // 6 half-intervals (on/off)
        unsigned long halfIdx = beepIndex;
        unsigned long halfDuration = 150;
        if (now - stateStart >= halfIdx * halfDuration) {
          // on-even, off-odd
          if ((halfIdx % 2) == 0) {
            int toneFreq = 2000 + (halfIdx / 2) * 400;
            ledcWriteTone(PWM_CH, toneFreq);
          } else {
            ledcWrite(PWM_CH, 0);
          }
          beepIndex++;
        }
      } else {
        // finished beeps -> next state
        ledcWrite(PWM_CH, 0);
        state = S_SWEEP;
        stateStart = now;
        sweepFreq = 400;
      }
      break;

    case S_SWEEP:
      // LED2 on during sweep, LED1 off
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, HIGH);

      // sweep from 400 to 3000 in steps of 100 every 20ms
      if (sweepFreq <= 3000) {
        unsigned long step = (sweepFreq - 400) / 100;
        if (now - stateStart >= step * 20) {
          ledcWriteTone(PWM_CH, sweepFreq);
          sweepFreq += 100;
        }
      } else {
        ledcWrite(PWM_CH, 0);
        // small pause after sweep
        state = S_MELODY;
        stateStart = now;
        melodyIndex = 0;
      }
      break;

    case S_MELODY:
      // both LEDs blink slowly during melody
      if (((now / 250) % 2) == 0) {
        digitalWrite(LED1_PIN, HIGH);
        digitalWrite(LED2_PIN, HIGH);
      } else {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
      }

      // play each note for 250ms
      if (melodyIndex < melodyLen) {
        unsigned long noteStart = stateStart + melodyIndex * 250UL;
        if (now >= noteStart) {
          ledcWriteTone(PWM_CH, melody[melodyIndex]);
          melodyIndex++;
        }
      } else {
        ledcWrite(PWM_CH, 0);
        // restart sequence
        state = S_BEEPS;
        stateStart = now;
        beepIndex = 0;
      }
      break;
  }
}