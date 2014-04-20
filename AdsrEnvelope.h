#ifndef ADSR_ENVELOPE_H
#define ADSR_ENVELOPE_H
/**
 * ADSR envelope class - Handles computation of a classical Attack-Decay-Sustain-Release envelope.
 *
 * Hugo Hromic - http://github.com/hhromic
 * MIT license
 */

#include <Arduino.h>

class AdsrEnvelope {
    public:
        AdsrEnvelope();
        void noteOn(unsigned long attackTime, unsigned long decayTime, float sustainLevel, unsigned long releaseTime);
        void noteOff(void);
        void tick(unsigned long time);
        float getOutput(void);
    private:
        enum States { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };
        struct Data {
            States state;
            unsigned long lastTime;
            float target;
            float attackRate;
            float decayStart;
            float decayRate;
            float sustainLevel;
            float releaseStart;
            float releaseRate;
            unsigned long releaseTime;
            float output;
        } data;
};

#endif
