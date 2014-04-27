#include <cmath>
#include <MidiLeds.h>

// Class constructor
MidiLeds::MidiLeds() {}

// Use LEDs array with given noteMin and noteMax limits
void MidiLeds::useLeds(struct CRGB *leds, uint8_t noteMin, uint8_t noteMax) {
    this->noteMin = noteMin;
    this->noteMax = noteMax;
    this->leds = leds;
    this->hsvData = new CHSV[noteMax - noteMin + 1];
    adsrEnvelopes = new AdsrEnvelope[noteMax - noteMin + 1];
    colorMapper = new MidiColorMapper(noteMin, noteMax);
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
    this->colorMapper->setMapper(colorMapper);
}
void MidiLeds::setNoteColorMap(MidiNoteColors::Maps noteColorMap) {
    parameters.noteColorMap = noteColorMap;
    colorMapper->setNoteColorMap(noteColorMap);
}
void MidiLeds::setFixedHue(uint8_t hue) {
    parameters.fixedHue = hue;
    colorMapper->setFixedHue(hue);
}
void MidiLeds::setIgnoreVelocity(bool state) { parameters.ignoreVelocity = state; }
void MidiLeds::setBaseBrightness(uint8_t value) { parameters.baseBrightness = value; }

// Process a MIDI Note On message
void MidiLeds::noteOn(uint8_t note, uint8_t velocity) {
    if (note >= noteMin && note <= noteMax) {
        hsvData[note - noteMin] = colorMapper->map(note, parameters.ignoreVelocity ? 0x7F : velocity);
        adsrEnvelopes[note - noteMin].noteOn(parameters.attackTime, parameters.decayTime, parameters.sustainLevel, parameters.releaseTime);
    }
}

// Process a MIDI Note Off message
void MidiLeds::noteOff(uint8_t note) {
    if (note >= noteMin && note <= noteMax)
        adsrEnvelopes[note - noteMin].noteOff();
}

// Process a MIDI All Notes Off message
void MidiLeds::allNotesOff(void) {
    for (size_t i=0; i<(noteMax - noteMin + 1); i++) {
        adsrEnvelopes[i].noteOff();
        hsvData[i] = CHSV(0,0,0);
    }
}

// Process a MIDI Reset All Controllers message
void MidiLeds::resetAllControllers(void) {
    parameters = DEFAULTS;
}

// Process a clock tick
void MidiLeds::tick(unsigned long time) {
    for (size_t i=0; i<(noteMax - noteMin + 1); i++) {
        if (adsrEnvelopes[i].isIdle())
            continue;
        adsrEnvelopes[i].tick(time);
        uint8_t brightness = round(adsrEnvelopes[i].getOutput() * hsvData[i].v);
        if (brightness < parameters.baseBrightness)
            brightness = parameters.baseBrightness;
        leds[i] = CHSV(hsvData[i].h, hsvData[i].v, brightness);
    }
}
