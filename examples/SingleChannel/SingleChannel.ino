/**
 * MIDI Leds example - Shows how all classes can be put together to create a nice MIDI Leds display controller.
 * Because I only have a Teensy 3.1 available for testing, it is not guaranteed to work on other platforms.
 * This Leds display controller also considers MIDI pedals for enhanced visualisation.
 *
 * This example uses only a single MIDI channel to work, making it simpler and easier to the eye.
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
#define DATA_PIN 2         // LED strip data pin
#define STATUS_LED_PIN 13  // Teensy 3.1 onboard LED
#define MIDI_CHANNEL 1     // What MIDI channel to listen for (1..16)?
#define NOTE_MIN 0x15      // note 21 (first note on standard 88 keys keyboard)
#define NOTE_MAX 0x6C      // note 108 (last note on standard 88 keys keyboard)
#define TIME_RANGE 5000    // Time range for setting parameters from MIDI control messages

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
MidiLeds midiLeds;
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
    
    // Init MidiLeds
    midiLeds.useLeds(leds, NOTE_MIN, NOTE_MAX);

    // Init pedals handlers
    damperPedal.setHandleNoteOn(damperNoteOn);
    damperPedal.setHandleNoteOff(damperNoteOff);
    softPedal.setHandleNoteOn(softNoteOn);
    sostenutoPedal.setHandleNoteOn(sostenutoNoteOn);
    sostenutoPedal.setHandleNoteOff(sostenutoNoteOff);

    // Init USB MIDI handlers
    usbMIDI.setHandleNoteOn(onNoteOn);
    usbMIDI.setHandleNoteOff(onNoteOff);
    usbMIDI.setHandleControlChange(onControlChange);
}

void loop() {
    digitalWrite(STATUS_LED_PIN, HIGH);
    usbMIDI.read();
    midiLeds.tick(elapsedTime);
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
    midiLeds.noteOn(note, velocity);
}

void sostenutoNoteOff(uint8_t channel, uint8_t note) {
    midiLeds.noteOff(note);
}

//***********************************************************************
// Message handlers for MIDI input (channel here comes in 1..16 range)

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (channel == MIDI_CHANNEL)
        damperPedal.noteOn(channel - 1, note, velocity);
    digitalWrite(STATUS_LED_PIN, LOW);
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (channel == MIDI_CHANNEL)
        damperPedal.noteOff(channel - 1, note);
    digitalWrite(STATUS_LED_PIN, LOW);
}

void onControlChange(uint8_t channel, uint8_t control, uint8_t value)  {
    if (channel == MIDI_CHANNEL) {
        switch (control) {
            case CC_COLOR_MAPPER:
                switch (value) {
                    case 0x00: midiLeds.setColorMapper(MidiColorMapper::COLOR_MAP); initNotes(); break;
                    case 0x01: midiLeds.setColorMapper(MidiColorMapper::RAINBOW); initNotes(); break;
                    case 0x02: midiLeds.setColorMapper(MidiColorMapper::FIXED_COLOR); initNotes(); break;
                }
                break;
            case CC_NOTE_COLOR_MAP:
                switch (value) {
                    case 0x00: midiLeds.setNoteColorMap(MidiNoteColors::AEPPLI_1940); initNotes(); break;
                    case 0x01: midiLeds.setNoteColorMap(MidiNoteColors::BELMONT_1944); initNotes(); break;
                    case 0x02: midiLeds.setNoteColorMap(MidiNoteColors::BERTRAND_1734); initNotes(); break;
                    case 0x03: midiLeds.setNoteColorMap(MidiNoteColors::BISHOP_1893); initNotes(); break;
                    case 0x04: midiLeds.setNoteColorMap(MidiNoteColors::FIELD_1816); initNotes(); break;
                    case 0x05: midiLeds.setNoteColorMap(MidiNoteColors::HELMHOLTZ_1910); initNotes(); break;
                    case 0x06: midiLeds.setNoteColorMap(MidiNoteColors::JAMESON_1844); initNotes(); break;
                    case 0x07: midiLeds.setNoteColorMap(MidiNoteColors::KLEIN_1930); initNotes(); break;
                    case 0x08: midiLeds.setNoteColorMap(MidiNoteColors::NEWTON_1704); initNotes(); break;
                    case 0x09: midiLeds.setNoteColorMap(MidiNoteColors::RIMINGTON_1893); initNotes(); break;
                    case 0x0A: midiLeds.setNoteColorMap(MidiNoteColors::SCRIABIN_1911); initNotes(); break;
                    case 0x0B: midiLeds.setNoteColorMap(MidiNoteColors::SEEMANN_1881); initNotes(); break;
                    case 0x0C: midiLeds.setNoteColorMap(MidiNoteColors::ZIEVERINK_2004); initNotes(); break;
                }
                break;
            case CC_FIXED_HUE: midiLeds.setFixedHue(round(0xFF * (value * 1.0f / 0x7F))); initNotes(); break;
            case CC_ATTACK_TIME: midiLeds.setAttackTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
            case CC_DECAY_TIME: midiLeds.setDecayTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
            case CC_SUSTAIN_LEVEL: midiLeds.setSustainLevel(1.0f * (value * 1.0f / 0x7F)); break;
            case CC_RELEASE_TIME: midiLeds.setReleaseTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
            case CC_IGNORE_VELOCITY: midiLeds.setIgnoreVelocity(value < 0x40 ? false : true); break;
            case CC_BASE_BRIGHTNESS: midiLeds.setBaseBrightness(value); initNotes(); break;
            case CC_ALL_NOTES_OFF: midiLeds.allNotesOff(); break;
            case CC_RESET_ALL_CONTROLLERS: midiLeds.resetAllControllers(); break;
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
    }
    digitalWrite(STATUS_LED_PIN, LOW);
}

//***********************************************************************
// Quickly send Note On and Note Off messages to re-init Leds

void initNotes() {
    for (uint8_t i=NOTE_MIN; i<=NOTE_MAX; i++) {
        midiLeds.noteOn(i, 0x00);
        midiLeds.noteOff(i);
    }
}
