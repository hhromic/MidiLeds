/**
 * MIDI Leds example - Shows how all classes can be put together to create a nice MIDI Leds display controller.
 * Because I only have a Teensy 3.1 available for testing, it is not guaranteed to work on other platforms.
 * This Leds display controller also considers MIDI pedals for enhanced visualisation.
 *
 * This example uses multiple MIDI channels to work. It handles parameters and pedalling independently for
 * each MIDI channel and it super-imposes LED visuals in reverse channel order (lower takes precedence).
 *
 * The event handling chain is as follows:
 * MIDI input -> Damper Pedal -> Soft Pedal -> Sostenuto Pedal -> MidiLeds
 *
 * Hugo Hromic - http://github.com/hhromic
 * MIT license
 */

#include <cmath> 
#include <Arduino.h>
#include <FastLED.h>
#include <MidiLeds.h>
#include <MidiDamperPedal.h>
#include <MidiSoftPedal.h>
#include <MidiSostenutoPedal.h>

// Program configuration
#define DATA_PIN 2          // LED strip data pin
#define STATUS_LED_PIN 13   // Teensy 3.1 onboard LED
#define NOTE_MIN 0x15       // note 21 (first note on standard 88 keys keyboard)
#define NOTE_MAX 0x6C       // note 108 (last note on standard 88 keys keyboard)
#define TIME_RANGE 5000     // Time range for setting parameters from MIDI control messages

// MIDI channels to listen (for now this is tricky, be careful when setting this section)
#define NUM_CHANNELS 9  // The number of total channels you will listen for (depends on RAM!)
#define CHANNELS 0b0000001011111111 // From right-to-left, put 1s or 0s to map MIDI channels
const size_t ML_INDEX[16] = { // From left-to-right put a correlative index (starts with 0)
  0, 1, 2, 3, 4, 5, 6, 7, -1, 8, -1, -1, -1, -1, -1 // you can use -1 to mark unused channels
};

// MIDI Control Change (CC) control bytes definitions
#define CC_COLOR_MAPPER           0x14
#define CC_NOTE_COLOR_MAP         0x15
#define CC_FIXED_HUE              0x16
#define CC_ATTACK_TIME            0x17
#define CC_DECAY_TIME             0x18
#define CC_SUSTAIN_LEVEL          0x19
#define CC_RELEASE_TIME           0x1A
#define CC_IGNORE_VELOCITY        0x1B
#define CC_BASE_BRIGHTNESS        0x1C
#define CC_ALL_NOTES_OFF          0x7B
#define CC_RESET_ALL_CONTROLLERS  0x79
#define CC_DAMPER_PEDAL           0x40
#define CC_SOFT_PEDAL             0x42
#define CC_SOSTENUTO_PEDAL        0x43

//***********************************************************************
// Global objects

elapsedMillis elapsedTime;
CRGB leds[NOTE_MAX - NOTE_MIN + 1];
MidiLeds midiLeds[NUM_CHANNELS];
MidiDamperPedal damperPedal;
MidiDamperPedal softPedal;
MidiDamperPedal sostenutoPedal;

//***********************************************************************
// Main setup and loop functions
// Make sure you check the FastLED.addLeds() function call.

void setup() {
    // Allow time for Leds power-up
    delay(2000);
    pinMode(STATUS_LED_PIN, OUTPUT);

    // Init FastLED
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NOTE_MAX - NOTE_MIN + 1);
    FastLED.setDither(0);
    FastLED.setCorrection(TypicalSMD5050);
    
    // Init pedals handlers
    damperPedal.setHandleNoteOn(damperNoteOn);
    damperPedal.setHandleNoteOff(damperNoteOff);
    softPedal.setHandleNoteOn(softNoteOn);
    sostenutoPedal.setHandleNoteOn(sostenutoNoteOn);
    sostenutoPedal.setHandleNoteOff(sostenutoNoteOff);

    // Init MidiLeds
    for (size_t i=0; i<NUM_CHANNELS; i++)
        midiLeds[i].useLeds(leds, NOTE_MIN, NOTE_MAX);

    // Init USB MIDI handlers
    usbMIDI.setHandleNoteOn(onNoteOn);
    usbMIDI.setHandleNoteOff(onNoteOff);
    usbMIDI.setHandleControlChange(onControlChange);
}

void loop() {
    digitalWrite(STATUS_LED_PIN, HIGH);
    usbMIDI.read();
    for (size_t i=0; i<NUM_CHANNELS; i++)
        midiLeds[NUM_CHANNELS - i - 1].tick(elapsedTime); // tick in reverse order
    FastLED.show();
}

//***********************************************************************
// Message handlers for the pedals

void damperNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    softPedal.noteOn(channel, note, velocity);
}

void damperNoteOff(uint8_t channel, uint8_t note) {
    sostenutoPedal.noteOff(channel, note);
}

void softNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    sostenutoPedal.noteOn(channel, note, velocity);
}

void sostenutoNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    midiLeds[ML_INDEX[channel]].noteOn(note, velocity);
}

void sostenutoNoteOff(uint8_t channel, uint8_t note) {
    midiLeds[ML_INDEX[channel]].noteOff(note);
}

//***********************************************************************
// Message handlers for MIDI input (MIDI channel comes in range 1..16)

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (!bitRead(CHANNELS, channel - 1))
        return;
    damperPedal.noteOn(channel - 1, note, velocity);
    digitalWrite(STATUS_LED_PIN, LOW);
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (!bitRead(CHANNELS, channel - 1))
        return;
    damperPedal.noteOff(channel - 1, note);
    digitalWrite(STATUS_LED_PIN, LOW);
}

void onControlChange(uint8_t channel, uint8_t control, uint8_t value)  {
    if (!bitRead(CHANNELS, channel - 1))
        return;
    size_t mlIndex = ML_INDEX[channel - 1];
    switch (control) {
        case CC_COLOR_MAPPER:
            switch (value) {
                case 0x00: midiLeds[mlIndex].setColorMapper(MidiColorMapper::COLOR_MAP); initNotes(mlIndex); break;
                case 0x01: midiLeds[mlIndex].setColorMapper(MidiColorMapper::RAINBOW); initNotes(mlIndex); break;
                case 0x02: midiLeds[mlIndex].setColorMapper(MidiColorMapper::FIXED_COLOR); initNotes(mlIndex); break;
            }
            break;
        case CC_NOTE_COLOR_MAP:
            switch (value) {
                case 0x00: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::AEPPLI_1940); initNotes(mlIndex); break;
                case 0x01: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::BELMONT_1944); initNotes(mlIndex); break;
                case 0x02: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::BERTRAND_1734); initNotes(mlIndex); break;
                case 0x03: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::BISHOP_1893); initNotes(mlIndex); break;
                case 0x04: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::FIELD_1816); initNotes(mlIndex); break;
                case 0x05: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::HELMHOLTZ_1910); initNotes(mlIndex); break;
                case 0x06: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::JAMESON_1844); initNotes(mlIndex); break;
                case 0x07: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::KLEIN_1930); initNotes(mlIndex); break;
                case 0x08: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::NEWTON_1704); initNotes(mlIndex); break;
                case 0x09: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::RIMINGTON_1893); initNotes(mlIndex); break;
                case 0x0A: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::SCRIABIN_1911); initNotes(mlIndex); break;
                case 0x0B: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::SEEMANN_1881); initNotes(mlIndex); break;
                case 0x0C: midiLeds[mlIndex].setNoteColorMap(MidiNoteColors::ZIEVERINK_2004); initNotes(mlIndex); break;
            }
            break;
        case CC_FIXED_HUE: midiLeds[mlIndex].setFixedHue(round(0xFF * (value * 1.0f / 0x7F))); initNotes(mlIndex); break;
        case CC_ATTACK_TIME: midiLeds[mlIndex].setAttackTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
        case CC_DECAY_TIME: midiLeds[mlIndex].setDecayTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
        case CC_SUSTAIN_LEVEL: midiLeds[mlIndex].setSustainLevel(1.0f * (value * 1.0f / 0x7F)); break;
        case CC_RELEASE_TIME: midiLeds[mlIndex].setReleaseTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
        case CC_IGNORE_VELOCITY: midiLeds[mlIndex].setIgnoreVelocity(value < 0x40 ? false : true); break;
        case CC_BASE_BRIGHTNESS: midiLeds[mlIndex].setBaseBrightness(value); initNotes(mlIndex); break;
        case CC_ALL_NOTES_OFF: midiLeds[mlIndex].allNotesOff(); break;
        case CC_RESET_ALL_CONTROLLERS: midiLeds[mlIndex].resetAllControllers(); break;
        case CC_DAMPER_PEDAL:
            if (value < 0x40) damperPedal.release(channel - 1);
            else damperPedal.press(channel - 1);
            break;
        case CC_SOFT_PEDAL:
            if (value < 0x40) softPedal.release(channel - 1);
            else softPedal.press(channel - 1);
            break;
        case CC_SOSTENUTO_PEDAL:
            if (value < 0x40) sostenutoPedal.release(channel - 1);
            else sostenutoPedal.press(channel - 1);
            break;
    }
    digitalWrite(STATUS_LED_PIN, LOW);
}

//***********************************************************************
// Quickly send Note On and Note Off messages to re-init Leds

void initNotes(size_t mlIndex) {
    for (uint8_t i=NOTE_MIN; i<=NOTE_MAX; i++) {
        midiLeds[mlIndex].noteOn(i, 0x00);
        midiLeds[mlIndex].noteOff(i);
    }
}
