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

#define DATA_PIN 2          // LED strip data pin
#define STATUS_LED_PIN 13   // Teensy 3.1 onboard LED
#define NOTE_MIN 0x15       // note 21 (first note on standard 88 keys keyboard)
#define NOTE_MAX 0x6C       // note 108 (last note on standard 88 keys keyboard)
#define CHANNEL_MIN 1       // from MIDI Channel 1
#define CHANNEL_MAX 8       // to MIDI Channel 8
#define TIME_RANGE 5000     // Time range for setting parameters from MIDI control messages
#define NUM_LEDS (NOTE_MAX - NOTE_MIN + 1)
#define NUM_CHANNELS (CHANNEL_MAX - CHANNEL_MIN + 1)
#define ML_NUM (channel - CHANNEL_MIN)

//***********************************************************************
// Global objects

elapsedMillis elapsedTime;
CRGB leds[NUM_LEDS];
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
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
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
    midiLeds[channel & 0xF].noteOn(note, velocity);
}

void sostenutoNoteOff(uint8_t channel, uint8_t note) {
    midiLeds[channel & 0xF].noteOff(note);
}

//***********************************************************************
// Message handlers for MIDI input (MIDI channel comes in range 1..16)

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (channel < CHANNEL_MIN || channel > CHANNEL_MAX)
        return;
    damperPedal.noteOn(channel - 1, note, velocity);
    digitalWrite(STATUS_LED_PIN, LOW);
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (channel < CHANNEL_MIN || channel > CHANNEL_MAX)
        return;
    damperPedal.noteOff(channel - 1, note);
    digitalWrite(STATUS_LED_PIN, LOW);
}

void onControlChange(uint8_t channel, uint8_t control, uint8_t value)  {
    if (channel < CHANNEL_MIN || channel > CHANNEL_MAX)
        return;
    switch (control) {
        case 0x00: // Bank Select
            switch (value) {
                case 0x00: midiLeds[ML_NUM].setColorMapper(MidiColorMapper::COLOR_MAP); initNotes(ML_NUM); break;
                case 0x01: midiLeds[ML_NUM].setColorMapper(MidiColorMapper::RAINBOW); initNotes(ML_NUM); break;
                case 0x02: midiLeds[ML_NUM].setColorMapper(MidiColorMapper::FIXED_COLOR); initNotes(ML_NUM); break;
            }
            break;
        case 0x20: // Bank Select (fine)
            switch (value) {
                case 0x00: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::AEPPLI_1940); initNotes(ML_NUM); break;
                case 0x01: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::BELMONT_1944); initNotes(ML_NUM); break;
                case 0x02: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::BERTRAND_1734); initNotes(ML_NUM); break;
                case 0x03: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::BISHOP_1893); initNotes(ML_NUM); break;
                case 0x04: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::FIELD_1816); initNotes(ML_NUM); break;
                case 0x05: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::HELMHOLTZ_1910); initNotes(ML_NUM); break;
                case 0x06: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::JAMESON_1844); initNotes(ML_NUM); break;
                case 0x07: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::KLEIN_1930); initNotes(ML_NUM); break;
                case 0x08: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::NEWTON_1704); initNotes(ML_NUM); break;
                case 0x09: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::RIMINGTON_1893); initNotes(ML_NUM); break;
                case 0x0A: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::SCRIABIN_1911); initNotes(ML_NUM); break;
                case 0x0B: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::SEEMANN_1881); initNotes(ML_NUM); break;
                case 0x0C: midiLeds[ML_NUM].setNoteColorMap(MidiNoteColors::ZIEVERINK_2004); initNotes(ML_NUM); break;
            }
            break;
        case 0x4D: // Vibrato Depth
            midiLeds[ML_NUM].setFixedHue(round(0xFF * (value * 1.0f / 0x7F))); initNotes(ML_NUM); break;
        case 0x49: // Attack Time
            midiLeds[ML_NUM].setAttackTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
        case 0x4B: // Decay Time
            midiLeds[ML_NUM].setDecayTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
        case 0x46: // Sound Variation
            midiLeds[ML_NUM].setSustainLevel(1.0f * (value * 1.0f / 0x7F)); break;
        case 0x48: // Release Time
            midiLeds[ML_NUM].setReleaseTime(round(TIME_RANGE * (value * 1.0f / 0x7F))); break;
        case 0x47: // Timbre Intensity
            midiLeds[ML_NUM].setIgnoreVelocity(value < 0x40 ? false : true); break;
        case 0x4A: // Brightness
            midiLeds[ML_NUM].setBaseBrightness(value); initNotes(ML_NUM); break;
        case 0x7B: // All Notes Off
            midiLeds[ML_NUM].allNotesOff(); break;
        case 0x79: // Reset All Controllers
            midiLeds[ML_NUM].resetAllControllers(); break;
        case 0x40: // Damper Pedal
            if (value < 0x40) damperPedal.release(channel - 1);
            else damperPedal.press(channel - 1);
            break;
        case 0x42: // Sostenuto Pedal
            if (value < 0x40) sostenutoPedal.release(channel - 1);
            else sostenutoPedal.press(channel - 1);
            break;
        case 0x43: // Soft Pedal
            if (value < 0x40) softPedal.release(channel - 1);
            else softPedal.press(channel - 1);
            break;
    }
    digitalWrite(STATUS_LED_PIN, LOW);
}

//***********************************************************************
// Quickly send Note On and Note Off messages to re-init Leds

void initNotes(uint8_t mlNum) {
    for (uint8_t i=NOTE_MIN; i<=NOTE_MAX; i++) {
        midiLeds[mlNum].noteOn(i, 0x00);
        midiLeds[mlNum].noteOff(i);
    }
}
