#ifndef MIDI_SOFT_PEDAL_H
#define MIDI_SOFT_PEDAL_H
/**
 * MIDI Soft Pedal class - Emulates a Soft Pedal using Note On messages.
 *
 * Hugo Hromic - http://github.com/hhromic
 * MIT license
 */

#include <cinttypes>
#include <Arduino.h>

class MidiSoftPedal {
    public:
        MidiSoftPedal();
        void setSoftenFactor(float factor);
        void press(uint8_t channel);
        void release(uint8_t channel);
        void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        void setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity));
    private:
        float softenFactor;
        uint16_t pressed;
        void (*handleNoteOn)(uint8_t channel, uint8_t note, uint8_t velocity);
};

#endif
