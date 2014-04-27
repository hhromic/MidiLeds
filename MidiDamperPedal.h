#ifndef MIDI_DAMPER_PEDAL_H
#define MIDI_DAMPER_PEDAL_H
/**
 * MIDI Damper Pedal class - Emulates a Damper Pedal using Note On/Off messages.
 *
 * Hugo Hromic - http://github.com/hhromic
 * MIT license
 */

#include <cinttypes>
#include <Arduino.h>

class MidiDamperPedal {
    public:
        MidiDamperPedal();
        void press(uint8_t channel);
        void release(uint8_t channel);
        void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        void noteOff(uint8_t channel, uint8_t note);
        void setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity));
        void setHandleNoteOff(void (*fptr)(uint8_t channel, uint8_t note));
    private:
        struct {
            bool pressed;
            bool heldNotes[128];
        } pedals[16];
        void (*handleNoteOn)(uint8_t channel, uint8_t note, uint8_t velocity);
        void (*handleNoteOff)(uint8_t channel, uint8_t note);
};

#endif
