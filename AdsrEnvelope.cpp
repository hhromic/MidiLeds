#include <cmath>
#include <AdsrEnvelope.h>

// Class constructor/initialisation
AdsrEnvelope::AdsrEnvelope() {
    data = {
        .state = AdsrEnvelope::IDLE,
        .output = 0.0f,
        .lastTime = 0U,
        .target = 0.0f,
        .attackRate = 0.0f,
        .decayStart = 0.0f,
        .decayRate = 0.0f,
        .sustainLevel = 0.0f,
        .releaseStart = 0.0f,
        .releaseRate = 0.0f,
        .releaseTime = 0U,
    };
}

// Start the ADSR envelope with given parameters
void AdsrEnvelope::noteOn(unsigned long attackTime, unsigned long decayTime, float sustainLevel, unsigned long releaseTime) {
    data.state = AdsrEnvelope::ATTACK;
    data.output = 0.0f;
    data.lastTime = 0U;
    data.target = 1.0f;
    data.attackRate = 1.0f / attackTime;
    data.decayRate = (1.0f - sustainLevel) / decayTime;
    data.sustainLevel = sustainLevel;
    data.releaseTime = releaseTime;
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
    if (data.state == AdsrEnvelope::IDLE)
        return;

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
                data.output = data.target;
                data.lastTime = 0U;
                data.decayStart = data.target;
                data.target = data.sustainLevel;
            }
            break;
        case AdsrEnvelope::DECAY: // Decay phase
            data.output = std::isinf(data.decayRate) ? data.target : data.decayStart - (relativeTime * data.decayRate);
            if (data.output <= data.target) { // Change to sustain phase?
                data.state = AdsrEnvelope::SUSTAIN;
                data.output = data.target;
                data.lastTime = 0U;
            }
            break;
        case AdsrEnvelope::SUSTAIN: // Sustain phase
            data.lastTime = 0U;
            if (data.output == 0.0) // Skip to idle phase?
                data.state = AdsrEnvelope::IDLE;
            break;
        case AdsrEnvelope::RELEASE: // Release phase
            data.output = std::isinf(data.releaseRate) ? data.target : data.releaseStart - (relativeTime * data.releaseRate);
            if (data.output <= data.target) { // Change to idle phase?
                data.state = AdsrEnvelope::IDLE;
                data.output = data.target;
                data.lastTime = 0U;
            }
            break;
    }
}

// Get the current ADSR envelope output value
float AdsrEnvelope::getOutput(void) {
    return data.output;
}

// Test if the ADSR envelope is in idle state
bool AdsrEnvelope::isIdle(void) {
    return data.state == AdsrEnvelope::IDLE;
}
