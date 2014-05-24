#ifndef MIDI_NOTE_COLORS_H
#define MIDI_NOTE_COLORS_H
/**
 * Note Color Maps - Different color mappings for MIDI notes.
 *
 * Hugo Hromic - http://github.com/hhromic
 * MIT license
 */

#include <cinttypes>
#include <Arduino.h>
#include <pixeltypes.h>

class MidiNoteColors {
    public:
        // Available note color maps
        enum Maps {
            AEPPLI_1940,
            BELMONT_1944,
            BERTRAND_1734,
            BISHOP_1893,
            FIELD_1816,
            HELMHOLTZ_1910,
            JAMESON_1844,
            KLEIN_1930,
            NEWTON_1704,
            RIMINGTON_1893,
            SCRIABIN_1911,
            SEEMANN_1881,
            ZIEVERINK_2004,
        };

        // Public methods
        static struct CHSV get(Maps map, uint8_t note);

    private:
        // Class constructor -- no need to instantiate this class
        MidiNoteColors();

        // Color data
        const static struct CHSV colorData[13][12];
};

#endif
