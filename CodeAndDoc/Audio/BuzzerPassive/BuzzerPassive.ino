/*
  The Tone library doesn't use the PWM mode timer functions on the atmega,
  instead it uses an interrupt routine to toggle the pins. So you can use
  it on all pins.
  
  Note: use of the tone() function will interfere with PWM output on pins 3
        and 11 (on boards other than the Mega).

  https://docs.arduino.cc/built-in-examples/digital/toneMelody
*/
#include "pitches.h"

const byte BUZZER_PIN = 8;

// Notes in the melody
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};

// Note durations: 4 = quarter note, 8 = eighth note, etc.
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

void setup()
{
  
}

void loop()
{
  // Iterate over the notes of the melody
  for (int thisNote = 0; thisNote < 8; thisNote++)
  {
    // To calculate the note duration, take one second divided by the note type
    // e.g. quarter note = 1000 / 4, eighth note = 1000 / 8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDuration);

    // To distinguish the notes, set a minimum time between them,
    // the note's duration + 30% seems to work well
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // Stop the tone playing
    noTone(BUZZER_PIN);
  }

  // Wait a moment
  delay(2000);
}
