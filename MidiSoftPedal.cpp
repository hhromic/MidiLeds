#include <cmath>
#include <MidiSoftPedal.h>

// Class constructor
MidiSoftPedal::MidiSoftPedal() {
    softenFactor = 2.0f / 3.0f;
    pedalPressed = false;
    handleNoteOn = NULL;
}

// Set the soften factor to apply to the note velocities
void MidiSoftPedal::setSoftenFactor(float factor) {
    softenFactor = factor;
}

// Emulate the pedal being pressed
void MidiSoftPedal::press(void) {
    pedalPressed = true;
}

// Emulate the pedal being released
void MidiSoftPedal::release(void) {
    pedalPressed = false;
}

// Process a MIDI Note On message
void MidiSoftPedal::noteOn(uint8_t note, uint8_t velocity) {
    uint8_t newVelocity = velocity;
    if (pedalPressed)
        newVelocity = round(velocity * softenFactor);
    handleNoteOn(note, newVelocity);
}

// Set a handler for processed Note On messages
void MidiSoftPedal::setHandleNoteOn(void (*fptr)(uint8_t note, uint8_t velocity)) {
    handleNoteOn = fptr;
}
