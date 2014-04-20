/**
 * MIDI Leds example - Shows how all classes can be put together to create a nice MIDI Leds display controller.
 * Because I only have a Teensy 3.1 available for testing, it is not guaranteed to work on other platforms.
 *
 * This Leds display controller also considers MIDI pedals for enhanced visualisation.
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

#define DATA_PIN 2          // LED strip data pin
#define STATUS_LED_PIN 13   // Teensy 3.1 onboard LED
#define NOTE_MIN 0x15       // note 21 (first note on standard 88 keys keyboard)
#define NOTE_MAX 0x6C       // note 108 (last note on standard 88 keys keyboard)
#define NOTES_CHANNEL 1     // What MIDI channel to listen for notes and pedals?
#define CONTROL_CHANNEL 16  // What MIDI channel to listen for parameter control?
#define TIME_RANGE 5000     // Time range for setting parameters from MIDI control messages
#define NUM_LEDS (NOTE_MAX - NOTE_MIN + 1)

//***********************************************************************
// Global objects

elapsedMillis elapsedTime;
CRGB leds[NUM_LEDS];
MidiLeds midiLeds(leds, NOTE_MIN, NOTE_MAX);
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
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setDither(0);
    FastLED.setCorrection(TypicalSMD5050);
    
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

void damperNoteOn(uint8_t note, uint8_t velocity) {
    softPedal.noteOn(note, velocity);
}

void damperNoteOff(uint8_t note) {
    sostenutoPedal.noteOff(note);
}

void softNoteOn(uint8_t note, uint8_t velocity) {
    sostenutoPedal.noteOn(note, velocity);
}

void sostenutoNoteOn(uint8_t note, uint8_t velocity) {
    midiLeds.noteOn(note, velocity);
}

void sostenutoNoteOff(uint8_t note) {
    midiLeds.noteOff(note);
}

//***********************************************************************
// Message handlers for MIDI input

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (channel == NOTES_CHANNEL)
        damperPedal.noteOn(note, velocity);
    digitalWrite(STATUS_LED_PIN, LOW);
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (channel == NOTES_CHANNEL)
        damperPedal.noteOff(note);
    digitalWrite(STATUS_LED_PIN, LOW);
}

void onControlChange(uint8_t channel, uint8_t control, uint8_t value)  {
    // Process control channel controls
    if (channel == CONTROL_CHANNEL) {
        switch (control) {
            case 0x00: // Bank Select
                switch (value) {
                    case 0x00: midiLeds.setColorMapper(MidiColorMapper::COLOR_MAP); initNotes(); break;
                    case 0x01: midiLeds.setColorMapper(MidiColorMapper::RAINBOW); initNotes(); break;
                }
                break;
            case 0x20: // Bank Select (fine)
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
            case 0x49: // Attack Time
                midiLeds.setAttackTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
            case 0x4B: // Decay Time
                midiLeds.setDecayTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
            case 0x46: // Sound Variation
                midiLeds.setSustainLevel(1.0f * (value * 1.0f / 0x7F)); break;
            case 0x48: // Release Time
                midiLeds.setReleaseTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
            case 0x47: // Timbre Intensity
                midiLeds.setIgnoreVelocity(value < 0x40 ? false : true); break;
            case 0x4A: // Brightness
                midiLeds.setBaseBrightness(value); initNotes(); break;
            case 0x7B: // All Notes Off
                midiLeds.allNotesOff(); break;
            case 0x79: // Reset All Controllers
                midiLeds.resetAllControllers(); break;
        }
    }
    
    // Process notes channel controls
    if (channel == NOTES_CHANNEL) {
        switch (control) {
            case 0x40: // Damper Pedal
                if (value < 0x40) damperPedal.release();
                else damperPedal.press();
                break;
            case 0x42: // Sostenuto Pedal
                if (value < 0x40) sostenutoPedal.release();
                else sostenutoPedal.press();
                break;
            case 0x43: // Soft Pedal
                if (value < 0x40) softPedal.release();
                else softPedal.press();
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
