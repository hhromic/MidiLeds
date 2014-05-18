#include <cmath>
#include <MidiColorMapper.h>

// Class constructor/initialisation
MidiColorMapper::MidiColorMapper() {
    for (uint8_t i=16; i--;)
        reset(i);
};

// Get the minimum note value for a MIDI channel
uint8_t MidiColorMapper::getNoteMin(uint8_t channel) {
    return parameters[channel & 0xF].noteMin;
}

// Set the minimum note value for a MIDI channel
void MidiColorMapper::setNoteMin(uint8_t channel, uint8_t noteMin) {
    parameters[channel & 0xF].noteMin = noteMin & 0x7F;
}

// Get the maximum note value for a MIDI channel
uint8_t MidiColorMapper::getNoteMax(uint8_t channel) {
    return parameters[channel & 0xF].noteMax;
}

// Set the maximum note value for a MIDI channel
void MidiColorMapper::setNoteMax(uint8_t channel, uint8_t noteMax) {
    parameters[channel & 0xF].noteMax = noteMax & 0x7F;
}

// Get the active color mapper for a MIDI channel
MidiColorMapper::Mappers MidiColorMapper::getMapper(uint8_t channel) {
    return parameters[channel & 0xF].mapper;
}

// Set the active color mapper to use for a MIDI channel
void MidiColorMapper::setMapper(uint8_t channel, Mappers mapper) {
    parameters[channel & 0xF].mapper = mapper;
}

// Get the active note color map for a MIDI channel
MidiNoteColors::Maps MidiColorMapper::getNoteColorMap(uint8_t channel) {
    return parameters[channel & 0xF].noteColorMap;
}

// Set the active note color map to use for a MIDI channel
void MidiColorMapper::setNoteColorMap(uint8_t channel, MidiNoteColors::Maps noteColorMap) {
    parameters[channel & 0xF].noteColorMap = noteColorMap;
}

// Get the active fixed color hue for a MIDI channel
uint8_t MidiColorMapper::getFixedHue(uint8_t channel) {
    return parameters[channel & 0xF].fixedHue;
}

// Set the active fixed color hue to use for a MIDI channel
void MidiColorMapper::setFixedHue(uint8_t channel, uint8_t fixedHue) {
    parameters[channel & 0xF].fixedHue = fixedHue;
}

// Get the active velocity ignoring state for a MIDI channel
bool MidiColorMapper::isIgnoreVelocity(uint8_t channel) {
    return parameters[channel & 0xF].ignoreVelocity;
}

// Set the active velocity ignoring state for a MIDI channel
void MidiColorMapper::setIgnoreVelocity(uint8_t channel, bool state) {
    parameters[channel & 0xF].ignoreVelocity = state;
}

// Map a MIDI note message to an HSV color
struct CHSV MidiColorMapper::map(uint8_t channel, uint8_t note, uint8_t velocity) {
    struct MidiColorMapperParameters *p = &parameters[channel & 0xF];
    struct CHSV noteColor = CHSV(0,0,0);
    if (note >= p->noteMin && note <= p->noteMax) {
        uint8_t _velocity = p->ignoreVelocity ? 0x7F : velocity & 0x7F;
        switch (p->mapper) {
            case COLOR_MAP: // Use Midi Note Color maps
                noteColor = MidiNoteColors::get(p->noteColorMap, note);
                noteColor.v = round((_velocity * 1.0f / 0x7F) * noteColor.v);
                break;
            case RAINBOW: // Generate rainbow-like colors
                noteColor = CHSV(
                    round((note - p->noteMin) * (0xFF / (p->noteMax - p->noteMin + 1.0f))),
                    0xFF, round((_velocity * 1.0f / 0x7F) * 0xFF)
                );
                break;
            case FIXED_COLOR: // Fixed color mapping
                noteColor = CHSV(p->fixedHue, 0xFF, round((_velocity * 1.0f / 0x7F) * 0xFF));
                break;
        }
    }
    return noteColor;
}

// Reset parameters values to defaults for a MIDI channel
void MidiColorMapper::reset(uint8_t channel) {
    parameters[channel & 0xF] = DEFAULTS;
}
