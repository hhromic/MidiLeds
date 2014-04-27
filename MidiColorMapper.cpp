#include <cmath>
#include <MidiColorMapper.h>

// Class constructor
MidiColorMapper::MidiColorMapper() {
    this->noteMin = 0x00;
    this->noteMax = 0x7F;
    this->mapper = COLOR_MAP;
    this->noteColorMap = MidiNoteColors::NEWTON_1704;
    this->fixedHue = 0x00;
}

// Set minimum note
void MidiColorMapper::setNoteMin(uint8_t noteMin) {
    this->noteMin = noteMin;
}

// Set maximum note
void MidiColorMapper::setNoteMax(uint8_t notemax) {
    this->noteMax = noteMax;
}

// Set the active color mapper to use
void MidiColorMapper::setMapper(Mappers mapper) {
    this->mapper = mapper;
}

// Set the active note color map to use
void MidiColorMapper::setNoteColorMap(MidiNoteColors::Maps noteColorMap) {
    this->noteColorMap = noteColorMap;
}

// Set the fixed color hue to use
void MidiColorMapper::setFixedHue(uint8_t hue) {
    this->fixedHue = hue;
}

// Map a MIDI note to an HSV color
struct CHSV MidiColorMapper::map(uint8_t note, uint8_t velocity) {
    struct CHSV noteColor;
    switch (mapper) {
        case COLOR_MAP: // Use Midi Note Color maps
            noteColor = MidiNoteColors::get(noteColorMap, note);
            noteColor.v = round((velocity * 1.0f / 0x7F) * noteColor.v);
            break;
        case RAINBOW: // Generate rainbow-like colors
            noteColor = CHSV(
                round((note - noteMin) * (0xFF / (noteMax - noteMin + 1.0f))),
                0xFF, round((velocity * 1.0f / 0x7F) * 0xFF)
            );
            break;
        case FIXED_COLOR: // Fixed color mapping
            noteColor = CHSV(fixedHue, 0xFF, round((velocity * 1.0f / 0x7F) * 0xFF));
            break;
    };
    return noteColor;
}
