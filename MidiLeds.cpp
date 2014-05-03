#include <cmath>
#include <MidiLeds.h>

// Class constructor
MidiLeds::MidiLeds() {
    useLeds(NULL, 0x00, 0x7F);
}

// Use LEDs array with given noteMin and noteMax limits
void MidiLeds::useLeds(struct CRGB *leds, uint8_t noteMin, uint8_t noteMax) {
    this->leds = leds;
    this->noteMin = noteMin;
    this->noteMax = noteMax;
    midiColorMapper.setNoteMin(noteMin);
    midiColorMapper.setNoteMax(noteMax);
    allNotesOff();
    resetAllControllers();
}

// Parameter getters
unsigned long MidiLeds::getAttackTime(void) { return parameters.attackTime; }
unsigned long MidiLeds::getDecayTime(void) { return parameters.decayTime; }
float MidiLeds::getSustainLevel(void) { return parameters.sustainLevel; }
unsigned long MidiLeds::getReleaseTime(void) { return parameters.releaseTime; }
MidiColorMapper::Mappers MidiLeds::getColorMapper(void) { return parameters.colorMapper; }
MidiNoteColors::Maps MidiLeds::getNoteColorMap(void) { return parameters.noteColorMap; }
uint8_t MidiLeds::getFixedHue(void) { return parameters.fixedHue; }
bool MidiLeds::getIgnoreVelocity(void) { return parameters.ignoreVelocity; }
uint8_t MidiLeds::getBaseBrightness(void) { return parameters.baseBrightness; }

// Parameter setters
void MidiLeds::setAttackTime(unsigned long attackTime) { parameters.attackTime = attackTime; }
void MidiLeds::setDecayTime(unsigned long decayTime) { parameters.decayTime = decayTime; }
void MidiLeds::setSustainLevel(float sustainLevel) { parameters.sustainLevel = sustainLevel; }
void MidiLeds::setReleaseTime(unsigned long releaseTime) { parameters.releaseTime = releaseTime; }
void MidiLeds::setColorMapper(MidiColorMapper::Mappers colorMapper) {
    parameters.colorMapper = colorMapper;
    midiColorMapper.setMapper(colorMapper);
}
void MidiLeds::setNoteColorMap(MidiNoteColors::Maps noteColorMap) {
    parameters.noteColorMap = noteColorMap;
    midiColorMapper.setNoteColorMap(noteColorMap);
}
void MidiLeds::setFixedHue(uint8_t hue) {
    parameters.fixedHue = hue;
    midiColorMapper.setFixedHue(hue);
}
void MidiLeds::setIgnoreVelocity(bool state) { parameters.ignoreVelocity = state; }
void MidiLeds::setBaseBrightness(uint8_t value) { parameters.baseBrightness = value; }

// Process a Note On message
void MidiLeds::noteOn(uint8_t note, uint8_t velocity) {
    if (note >= noteMin && note <= noteMax) {
        hsvData[note] = midiColorMapper.map(note, parameters.ignoreVelocity ? 0x7F : velocity);
        adsrEnvelopes[note].noteOn(parameters.attackTime, parameters.decayTime, parameters.sustainLevel, parameters.releaseTime);
    }
}

// Process a Note Off message
void MidiLeds::noteOff(uint8_t note) {
    if (note >= noteMin && note <= noteMax)
        adsrEnvelopes[note].noteOff();
}

// Turn off all Leds
void MidiLeds::allLedsOff(void) {
    for (size_t i=0; i<0x7F; i++) {
        adsrEnvelopes[i].noteOff();
        hsvData[i] = CHSV(0,0,0);
    }
}

// Reset all parameters to their defaults
void MidiLeds::reset(void) {
    parameters = DEFAULTS;
}

// Process a clock tick
void MidiLeds::tick(unsigned long time) {
    for (size_t i=noteMin; i<=noteMax; i++) {
        if (adsrEnvelopes[i].tick(time)) {
            uint8_t brightness = round(adsrEnvelopes[i].getOutput() * hsvData[i].v);
            if (brightness < parameters.baseBrightness)
                brightness = parameters.baseBrightness;
            leds[i - noteMin] = CHSV(hsvData[i].h, hsvData[i].v, brightness);
        }
    }
}
