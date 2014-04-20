#ifndef MIDI_SOSTENUTO_PEDAL_H
#define MIDI_SOSTENUTO_PEDAL_H
/**
 * MIDI Sostenuto Pedal class - Emulates a Sostenuto Pedal using Note On/Off messages.
 *
 * Hugo Hromic - http://github.com/hhromic
 * MIT license
 */

#include <cinttypes>
#include <Arduino.h>

class MidiSostenutoPedal {
    public:
        MidiSostenutoPedal();
        void press(void);
        void release(void);
        void noteOn(uint8_t note, uint8_t velocity);
        void noteOff(uint8_t note);
        void setHandleNoteOn(void (*fptr)(uint8_t note, uint8_t velocity));
        void setHandleNoteOff(void (*fptr)(uint8_t note));
    private:
        bool pedalPressed;
        bool prePedalNotes[128];
        bool pedalNotes[128];
        bool heldNotes[128];
        void (*handleNoteOn)(uint8_t note, uint8_t velocity);
        void (*handleNoteOff)(uint8_t note);
};

#endif
