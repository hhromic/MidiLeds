#include <cmath>
#include <MidiSoftPedal.h>

// Class constructor
MidiSoftPedal::MidiSoftPedal() {
    softenFactor = 2.0f / 3.0f;
    for (size_t i=0; i<16; i++)
        pedals[i].pressed = false;
    handleNoteOn = NULL;
}

// Set the soften factor to apply to the note velocities
void MidiSoftPedal::setSoftenFactor(float factor) {
    softenFactor = factor;
}

// Emulate the pedal being pressed
void MidiSoftPedal::press(uint8_t channel) {
    pedals[channel & 0xF].pressed = true;
}

// Emulate the pedal being released
void MidiSoftPedal::release(uint8_t channel) {
    pedals[channel & 0xF].pressed = false;
}

// Process a MIDI Note On message
void MidiSoftPedal::noteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (pedals[channel & 0xF].pressed)
        handleNoteOn(channel, note, round(velocity * softenFactor));
    else
        handleNoteOn(channel, note, velocity);
}

// Set a handler for processed Note On messages
void MidiSoftPedal::setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity)) {
    handleNoteOn = fptr;
}
