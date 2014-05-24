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
        // Class constructor
        AdsrEnvelope();

        // Public methods
        void noteOn(unsigned long attackTime, unsigned long decayTime, float sustainLevel, unsigned long releaseTime);
        void noteOff(void);
        void tick(unsigned long time);
        float getOutput(void);
        bool isIdle(void);

    private:
        // Possible envelope states
        enum States { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };

        // Internal envelope data
        struct Data {
            States state;
            float output;
            unsigned long lastTime;
            float target;
            float attackRate;
            float decayStart;
            float decayRate;
            float sustainLevel;
            float releaseStart;
            float releaseRate;
            unsigned long releaseTime;
        } data;
};

#endif
