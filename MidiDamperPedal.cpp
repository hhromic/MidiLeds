#include <MidiDamperPedal.h>

// Class constructor
MidiDamperPedal::MidiDamperPedal() {
    pressed = 0x0000;
    for (size_t i=0; i<16; i++)
        for (size_t j=0; j<4; j++)
            heldNotes[i][j] = 0x00000000;
    handleNoteOn = NULL;
    handleNoteOff = NULL;
}

// Emulate the pedal being pressed
void MidiDamperPedal::press(uint8_t channel) {
    bitSet(pressed, channel & 0xF);
}

// Emulate the pedal being released
void MidiDamperPedal::release(uint8_t channel) {
    bitClear(pressed, channel & 0xF);
    for (size_t i=0; i<128; i++) { // Send Note Off messages for all channel held notes
        if (bitRead(heldNotes[channel & 0xF][i / 32], i % 32)) {
            bitClear(heldNotes[channel & 0xF][i / 32], i % 32);
            handleNoteOff(channel, i);
        }
    }
}

// Process a MIDI Note On message
void MidiDamperPedal::noteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (bitRead(heldNotes[channel & 0xF][(note & 0x7F) / 32], (note & 0x7F) % 32))
        bitClear(heldNotes[channel & 0xF][(note & 0x7F) / 32], (note & 0x7F) % 32); // Reset channel held note
    handleNoteOn(channel, note, velocity);
}

// Process a MIDI Note Off message
void MidiDamperPedal::noteOff(uint8_t channel, uint8_t note) {
    if (bitRead(pressed, channel & 0xF)) // If pedal pressed, hold channel Note Off message
        bitSet(heldNotes[channel & 0xF][(note & 0x7F) / 32], (note & 0x7F) % 32);
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
