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
        // Class constructor
        MidiSoftPedal();

        // Getter/setters
        float getSoftenFactor(void);
        void setSoftenFactor(float factor);
        
        // Public methods
        void press(uint8_t channel);
        void release(uint8_t channel);
        void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        void setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity));

    private:
        // Internal bit-wise states and parameters
        float softenFactor;
        uint16_t pressed;

        // MIDI message handlers
        void (*handleNoteOn)(uint8_t channel, uint8_t note, uint8_t velocity);
};

#endif
