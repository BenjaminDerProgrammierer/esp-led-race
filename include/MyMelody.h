/**
 * MyMelody.h
 * Benjamin Hartmann | 12/2025
 */

#ifndef _MY_MELODY_H_
#define _MY_MELODY_H_

#include "Arduino.h"
#include "pitches.h"

class MyMelody {
   private:
    int melody[8] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};
    int duration = 1000;
    uint8_t _pin;
   public:
      MyMelody(int pin) : _pin(pin) {}
      void playNote(int note) {
         tone(_pin, melody[note], duration);
         delay(duration * 1.30);
      }
};
#endif  // _MY_MELODY_H_
