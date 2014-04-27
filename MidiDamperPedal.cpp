#include <MidiDamperPedal.h>

// Class constructor
MidiDamperPedal::MidiDamperPedal() {
    for (size_t i=0; i<16; i++) {
        pedals[i].pressed = false;
        for (size_t j=0; j<128; j++)
            pedals[i].heldNotes[j] = false;
    }
    handleNoteOn = NULL;
    handleNoteOff = NULL;
}

// Emulate the pedal being pressed
void MidiDamperPedal::press(uint8_t channel) {
    pedals[channel & 0xF].pressed = true;
}

// Emulate the pedal being released
void MidiDamperPedal::release(uint8_t channel) {
    pedals[channel & 0xF].pressed = false;
    for (size_t i=0; i<128; i++) {
        if (pedals[channel & 0xF].heldNotes[i]) {
            pedals[channel & 0xF].heldNotes[i] = false;
            handleNoteOff(channel, i);
        }
    }
}

// Process a MIDI Note On message
void MidiDamperPedal::noteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (pedals[channel & 0xF].heldNotes[note & 0x7F])
        pedals[channel & 0xF].heldNotes[note & 0x7F] = false;
    handleNoteOn(channel, note, velocity);
}

// Process a MIDI Note Off message
void MidiDamperPedal::noteOff(uint8_t channel, uint8_t note) {
    if (pedals[channel & 0xF].pressed)
        pedals[channel & 0xF].heldNotes[note & 0x7F] = true;
    else
        handleNoteOff(channel, note);
}

// Set a handler for processed Note On messages
void MidiDamperPedal::setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity)) {
    handleNoteOn = fptr;
}

// Set a handler for processed Note Off messages
void MidiDamperPedal::setHandleNoteOff(void (*fptr)(uint8_t channel, uint8_t note)) {
    handleNoteOff = fptr;
}
