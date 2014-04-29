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
        void press(uint8_t channel);
        void release(uint8_t channel);
        void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        void noteOff(uint8_t channel, uint8_t note);
        void setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity));
        void setHandleNoteOff(void (*fptr)(uint8_t channel, uint8_t note));
    private:
        uint16_t pressed;
        uint32_t prePedalNotes[16][4];
        uint32_t pedalNotes[16][4];
        uint32_t heldNotes[16][4];
        void (*handleNoteOn)(uint8_t channel, uint8_t note, uint8_t velocity);
        void (*handleNoteOff)(uint8_t channel, uint8_t note);
};

#endif
