#ifndef MIDI_COLOR_MAPPER_H
#define MIDI_COLOR_MAPPER_H
/**
 * Midi Color Mapper class - Handles mapping of MIDI notes to colors.
 *
 * Hugo Hromic - http://github.com/hhromic
 * MIT license
 */

#include <cinttypes>
#include <Arduino.h>
#include <MidiNoteColors.h>
#include <pixeltypes.h>

class MidiColorMapper {
    public:
        enum Mappers { COLOR_MAP, RAINBOW, FIXED_COLOR };
        MidiColorMapper(uint8_t noteMin, uint8_t noteMax);
        void setMapper(Mappers mapper);
        void setNoteColorMap(MidiNoteColors::Maps noteColorMap);
        void setFixedHue(uint8_t hue);
        struct CHSV map(uint8_t note, uint8_t velocity);
    private:
        uint8_t noteMin;
        uint8_t noteMax;
        Mappers mapper;
        MidiNoteColors::Maps noteColorMap;
        uint8_t fixedHue;
};

#endif
