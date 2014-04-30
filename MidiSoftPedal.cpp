#include <cmath>
#include <MidiSoftPedal.h>

// Class constructor
MidiSoftPedal::MidiSoftPedal() {
    softenFactor = 2.0f / 3.0f;
    pressed = 0x0000;
    handleNoteOn = NULL;
}

// Set the soften factor to apply to the note velocities (must be <= 1)
void MidiSoftPedal::setSoftenFactor(float factor) {
    if (softenFactor >= 0.0f && softenFactor <= 1.0f)
        softenFactor = factor;
}

// Emulate the pedal being pressed
void MidiSoftPedal::press(uint8_t channel) {
    bitSet(pressed, channel & 0xF);
}

// Emulate the pedal being released
void MidiSoftPedal::release(uint8_t channel) {
    bitClear(pressed, channel & 0xF);
}

// Process a MIDI Note On message
void MidiSoftPedal::noteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (bitRead(pressed, channel & 0xF))
        handleNoteOn(channel, note, round(velocity * softenFactor));
    else
        handleNoteOn(channel, note, velocity);
}

// Set a handler for processed Note On messages
void MidiSoftPedal::setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity)) {
    handleNoteOn = fptr;
}
