#include <MidiSostenutoPedal.h>

// Class constructor
MidiSostenutoPedal::MidiSostenutoPedal() {
    pedalPressed = false;
    for (size_t i=0; i<128; i++) {
        prePedalNotes[i] = false;
        pedalNotes[i] = false;
        heldNotes[i] = false;
    }
    handleNoteOn = NULL;
    handleNoteOff = NULL;
}

// Emulate the pedal being pressed
void MidiSostenutoPedal::press(void) {
    pedalPressed = true;
    for (size_t i=0; i<128; i++)
        if (prePedalNotes[i])
            pedalNotes[i] = true;
}

// Emulate the pedal being released
void MidiSostenutoPedal::release(void) {
    pedalPressed = false;
    for (size_t i=0; i<128; i++) {
        pedalNotes[i] = false;
        if (heldNotes[i]) {
            heldNotes[i] = false;
            handleNoteOff(i);
        }
    }
}

// Process a MIDI Note On message
void MidiSostenutoPedal::noteOn(uint8_t note, uint8_t velocity) {
    prePedalNotes[note & 0x7F] = true;
    if (pedalPressed)
        heldNotes[note & 0x7F] = false;
    handleNoteOn(note, velocity);
}

// Process a MIDI Note Off message
void MidiSostenutoPedal::noteOff(uint8_t note) {
    prePedalNotes[note & 0x7F] = false;
    if (pedalPressed && pedalNotes[note & 0x7F])
        heldNotes[note & 0x7F] = true;
    else
        handleNoteOff(note);
}

// Set a handler for processed Note On messages
void MidiSostenutoPedal::setHandleNoteOn(void (*fptr)(uint8_t note, uint8_t velocity)) {
    handleNoteOn = fptr;
}

// Set a handler for processed Note Off messages
void MidiSostenutoPedal::setHandleNoteOff(void (*fptr)(uint8_t note)) {
    handleNoteOff = fptr;
}