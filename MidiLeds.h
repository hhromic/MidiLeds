#ifndef MIDILEDS_H
#define MIDILEDS_H
/**
 * MIDI Leds class - Translates MIDI Note On/Off messages into RGB Leds data.
 * Couple this with MIDI Soft/Damper/Sostenuto Pedals and get accurate looking lights.
 *
 * Hugo Hromic - http://github.com/hhromic
 * MIT license
 */

#include <cinttypes>
#include <Arduino.h>
#include <pixeltypes.h>
#include <AdsrEnvelope.h>
#include <MidiColorMapper.h>
#include <MidiNoteColors.h>

class MidiLeds {
    public:
        MidiLeds();

        // Configuration
        void useLeds(struct CRGB *leds, uint8_t noteMin, uint8_t noteMax);

        // Parameter getters
        unsigned long getAttackTime(void);
        unsigned long getDecayTime(void);
        float getSustainLevel(void);
        unsigned long getReleaseTime(void);
        MidiColorMapper::Mappers getColorMapper(void);
        MidiNoteColors::Maps getNoteColorMap(void);
        uint8_t getFixedHue(void);
        bool getIgnoreVelocity(void);
        uint8_t getBaseBrightness(void);

        // Parameter setters
        void setAttackTime(unsigned long attackTime);
        void setDecayTime(unsigned long decayTime);
        void setSustainLevel(float sustainLevel);
        void setReleaseTime(unsigned long releaseTime);
        void setColorMapper(MidiColorMapper::Mappers colorMapper);
        void setNoteColorMap(MidiNoteColors::Maps noteColorMap);
        void setFixedHue(uint8_t hue);
        void setIgnoreVelocity(bool state);
        void setBaseBrightness(uint8_t value);

        // Event handlers
        void noteOn(uint8_t note, uint8_t velocity);
        void noteOff(uint8_t note);
        void allNotesOff(void);
        void resetAllControllers(void);
        void tick(unsigned long time);
    private:
        uint8_t noteMin;
        uint8_t noteMax;
        struct CRGB *leds;
        struct CHSV *hsvData;
        AdsrEnvelope *adsrEnvelopes;
        MidiColorMapper *colorMapper;
        struct MidiLedsParameters {
            unsigned long attackTime;
            unsigned long decayTime;
            float sustainLevel;
            unsigned long releaseTime;
            MidiColorMapper::Mappers colorMapper;
            MidiNoteColors::Maps noteColorMap;
            uint8_t fixedHue;
            bool ignoreVelocity;
            uint8_t baseBrightness;
        } parameters;
        const struct MidiLedsParameters DEFAULTS = {
            .attackTime = 80U,
            .decayTime = 3000U,
            .sustainLevel = 0.0,
            .releaseTime = 400U,
            .colorMapper = MidiColorMapper::COLOR_MAP,
            .noteColorMap = MidiNoteColors::NEWTON_1704,
            .fixedHue = 0x00,
            .ignoreVelocity = true,
            .baseBrightness = 0x00,
        };
};

#endif
