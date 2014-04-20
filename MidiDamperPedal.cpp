#include <MidiDamperPedal.h>

// Class constructor
MidiDamperPedal::MidiDamperPedal() {
    pedalPressed = false;
    for (size_t i=0; i<128; i++)
        heldNotes[i] = false;
    handleNoteOn = NULL;
    handleNoteOff = NULL;
}

// Emulate the pedal being pressed
void MidiDamperPedal::press(void) {
    pedalPressed = true;
}

// Emulate the pedal being released
void MidiDamperPedal::release(void) {
    pedalPressed = false;
    for (size_t i=0; i<128; i++) {
        if (heldNotes[i]) {
            heldNotes[i] = false;
            handleNoteOff(i);
        }
    }
}

// Process a MIDI Note On message
void MidiDamperPedal::noteOn(uint8_t note, uint8_t velocity) {
    if (heldNotes[note & 0x7F])
        heldNotes[note & 0x7F] = false;
    handleNoteOn(note, velocity);
}

// Process a MIDI Note Off message
void MidiDamperPedal::noteOff(uint8_t note) {
    if (pedalPressed)
        heldNotes[note & 0x7F] = true;
    else
        handleNoteOff(note);
}

// Set a handler for processed Note On messages
void MidiDamperPedal::setHandleNoteOn(void (*fptr)(uint8_t note, uint8_t velocity)) {
    handleNoteOn = fptr;
}

// Set a handler for processed Note Off messages
void MidiDamperPedal::setHandleNoteOff(void (*fptr)(uint8_t note)) {
    handleNoteOff = fptr;
}
