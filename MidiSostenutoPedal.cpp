#include <MidiSostenutoPedal.h>

// Class constructor
MidiSostenutoPedal::MidiSostenutoPedal() {
    pressed = 0x0000;
    for (size_t i=0; i<16; i++)
        for (size_t j=0; j<4; j++) {
            prePedalNotes[i][j] = 0x00000000;
            pedalNotes[i][j] = 0x00000000;
            heldNotes[i][j] = 0x00000000;
        }
    handleNoteOn = NULL;
    handleNoteOff = NULL;
}

// Emulate the pedal being pressed
void MidiSostenutoPedal::press(uint8_t channel) {
    bitSet(pressed, channel & 0xF);
    for (size_t i=0; i<4; i++) // Transfer all channel pre-pedal notes to pedal notes
        pedalNotes[channel & 0xF][i] = prePedalNotes[channel & 0xF][i];
}

// Emulate the pedal being released
void MidiSostenutoPedal::release(uint8_t channel) {
    bitClear(pressed, channel & 0xF);
    for (size_t i=0; i<128; i++) { // Send Note Off messages for all channel held notes
        bitClear(pedalNotes[channel & 0xF][i / 32], i % 32); // Reset channel pedal notes
        if (bitRead(heldNotes[channel & 0xF][i / 32], i % 32)) {
            bitClear(heldNotes[channel & 0xF][i / 32], i % 32);
            handleNoteOff(channel, i, 0x00);
        }
    }
}

// Process a MIDI Note On message
void MidiSostenutoPedal::noteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    bitSet(prePedalNotes[channel & 0xF][(note & 0x7F) / 32], (note & 0x7F) % 32); // Remember as channel pre-pedal note
    if (bitRead(pressed, channel & 0xF)) // If pedal pressed, reset channel held note
        bitClear(heldNotes[channel & 0xF][(note & 0x7F) / 32], (note & 0x7F) % 32);
    handleNoteOn(channel, note, velocity);
}

// Process a MIDI Note Off message
void MidiSostenutoPedal::noteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    bitClear(prePedalNotes[channel & 0xF][(note & 0x7F) / 32], (note & 0x7F) % 32); // Reset channel pre-pedal note
    if (bitRead(pressed, channel & 0xF) && bitRead(pedalNotes[channel & 0xF][(note & 0x7F) / 32], (note & 0x7F) % 32))
        bitSet(heldNotes[channel & 0xF][(note & 0x7F) / 32], (note & 0x7F) % 32);
    else
        handleNoteOff(channel, note, velocity);
}

// Set a handler for processed Note On messages
void MidiSostenutoPedal::setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity)) {
    handleNoteOn = fptr;
}

// Set a handler for processed Note Off messages
void MidiSostenutoPedal::setHandleNoteOff(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity)) {
    handleNoteOff = fptr;
}
