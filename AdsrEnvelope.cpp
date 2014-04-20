#include <cmath>
#include <AdsrEnvelope.h>

// Class constructor
AdsrEnvelope::AdsrEnvelope() {
    // Initialise the new ADSR envelope
    data = {
        .state = AdsrEnvelope::IDLE,
        .lastTime = 0U,
        .target = 0.0f,
        .attackRate = 0.0f,
        .decayStart = 0.0f,
        .decayRate = 0.0f,
        .sustainLevel = 0.0f,
        .releaseStart = 0.0f,
        .releaseRate = 0.0f,
        .releaseTime = 0U,
        .output = 0.0f
    };
}

// Start the ADSR envelope with given parameters
void AdsrEnvelope::noteOn(unsigned long attackTime, unsigned long decayTime, float sustainLevel, unsigned long releaseTime) {
    data.state = AdsrEnvelope::ATTACK;
    data.lastTime = 0U;
    data.target = 1.0f;
    data.attackRate = 1.0f / attackTime;
    data.decayRate = (1.0f - sustainLevel) / decayTime;
    data.sustainLevel = sustainLevel;
    data.releaseTime = releaseTime;
    data.output = 0.0f;
}

// Trigger the release phase of the ADSR envelope
void AdsrEnvelope::noteOff(void) {
    if (data.state != AdsrEnvelope::IDLE) {
        data.state = AdsrEnvelope::RELEASE;
        data.lastTime = 0U;
        data.target = 0.0f;
        data.releaseStart = data.output;
        data.releaseRate = data.releaseStart / data.releaseTime;
    }
}

// Update the ADSR envelope phase and output value (assumes monotonically increasing time)
void AdsrEnvelope::tick(unsigned long time) {
    // Do nothing if the envelope is idle
    if (data.state == AdsrEnvelope::IDLE) return;

    // Handle envelope relative time
    if (data.lastTime == 0U)
        data.lastTime = time;
    unsigned long relativeTime = time - data.lastTime;

    // Update envelope state and output
    switch (data.state) {
        case AdsrEnvelope::ATTACK: // Attack phase
            data.output = std::isinf(data.attackRate) ? data.target : relativeTime * data.attackRate;
            if (data.output >= data.target) { // Change to decay phase?
                data.state = AdsrEnvelope::DECAY;
                data.lastTime = 0U;
                data.decayStart = data.target;
                data.output = data.target;
                data.target = data.sustainLevel;
            }
            return;
        case AdsrEnvelope::DECAY: // Decay phase
            data.output = std::isinf(data.decayRate) ? data.target : data.decayStart - (relativeTime * data.decayRate);
            if (data.output <= data.target) { // Change to sustain phase?
                data.state = AdsrEnvelope::SUSTAIN;
                data.lastTime = 0U;
                data.output = data.target;
            }
            return;
        case AdsrEnvelope::SUSTAIN: // Sustain phase
            data.lastTime = 0U;
            return;
        case AdsrEnvelope::RELEASE: // Release phase
            data.output = std::isinf(data.releaseRate) ? data.target : data.releaseStart - (relativeTime * data.releaseRate);
            if (data.output <= data.target) { // Change to idle phase?
                data.state = AdsrEnvelope::IDLE;
                data.lastTime = 0U;
                data.output = data.target;
            }
            return;
    }
}

// Get the current ADSR envelope output value
float AdsrEnvelope::getOutput(void) {
    return data.output;
}
