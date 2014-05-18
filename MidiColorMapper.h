#ifndef MIDI_COLOR_MAPPER_H
#define MIDI_COLOR_MAPPER_H
/**
 * Midi Color Mapper class - Handles mapping of MIDI notes to colors.
 *
 * Hugo Hromic - http://github.com/hhromic
 * MIT license
 */

#include <Arduino.h>
#include <cinttypes>
#include <MidiNoteColors.h>
#include <pixeltypes.h>

class MidiColorMapper {
    public:
        // Available color mappers
        enum Mappers { COLOR_MAP, RAINBOW, FIXED_COLOR };
        
        // Class constructor
        MidiColorMapper();
        
        // Getter/setters
        uint8_t getNoteMin(uint8_t channel);
        void setNoteMin(uint8_t channel, uint8_t noteMin);
        uint8_t getNoteMax(uint8_t channel);
        void setNoteMax(uint8_t channel, uint8_t noteMax);
        Mappers getMapper(uint8_t channel);
        void setMapper(uint8_t channel, Mappers mapper);
        MidiNoteColors::Maps getNoteColorMap(uint8_t channel);
        void setNoteColorMap(uint8_t channel, MidiNoteColors::Maps noteColorMap);
        uint8_t getFixedHue(uint8_t channel);
        void setFixedHue(uint8_t channel, uint8_t fixedHue);
        bool isIgnoreVelocity(uint8_t channel);
        void setIgnoreVelocity(uint8_t channel, bool state);
        
        // Public methods
        struct CHSV map(uint8_t channel, uint8_t note, uint8_t velocity);
        void reset(uint8_t channel);

    private:
        // Parameters per MIDI channel
        struct MidiColorMapperParameters {
            uint8_t noteMin;
            uint8_t noteMax;
            Mappers mapper;
            MidiNoteColors::Maps noteColorMap;
            uint8_t fixedHue;
            bool ignoreVelocity;
        } parameters[16];
        
        // Default parameters settings
        const struct MidiColorMapperParameters DEFAULTS = {
            .noteMin = 0x00,
            .noteMax = 0x7F,
            .mapper = COLOR_MAP,
            .noteColorMap = MidiNoteColors::NEWTON_1704,
            .fixedHue = 0x00,
            .ignoreVelocity = true,
        };
};

#endif
