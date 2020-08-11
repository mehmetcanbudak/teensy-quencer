//
// Created by Nicholas Newdigate on 06/08/2020.
//

#ifndef TEENSYSEQUENCER_POLYPHONICSAMPLER_H
#define TEENSYSEQUENCER_POLYPHONICSAMPLER_H

#include <functional>
#include <vector>
#include "sequencer.h"

using namespace std;

class polyphonicsampler {
public:
    polyphonicsampler() : noteOnFns() {
        for (int i=0; i < 128; i++)
          activeNotes[i] = 255;
        for (int i=0; i < 16; i++)
          activeVoices[i] = 255;
    }

    void noteOn(uint8_t noteNumber, uint8_t velocity) {
        if (activeNotes[noteNumber] == 255) {
            // note is not active
            int nextFreeVoice = getFirstFreeVoice();
            if (nextFreeVoice < noteOnFns.size()) {
                activeVoices[nextFreeVoice] = noteNumber;
                activeNotes[noteNumber] = nextFreeVoice;
                if ( nextFreeVoice < noteOnFns.size() ){
                    function<void(uint8_t noteNumber, uint8_t velocity, bool isNoteOn)> noteOnFunction = noteOnFns[nextFreeVoice];
                    noteOnFunction(noteNumber, velocity, true);
                }
            }
        } else {
            // note is already active, just re-trigger it...
        }
    }

    void noteOff(uint8_t noteNumber) {
        uint8_t index = activeNotes[noteNumber];
        if (index == 255) {
            // note is not active, ignore
            return;
        }

        function<void(uint8_t noteNumber, uint8_t velocity, bool isNoteOn)> noteOnFunction = noteOnFns[index];
        noteOnFunction(noteNumber, 0, false);
        activeNotes[noteNumber] = 255;
        activeVoices[index] = 255; // free the voice
    }

    void pushNoteFunction (const function<void(uint8_t noteNumber, uint8_t velocity, bool isNoteOn)> &noteOnFunction) {
        noteOnFns.push_back(  noteOnFunction );
    }

private:
    vector<function<void(uint8_t noteNumber, uint8_t velocity, bool isNoteOn)>> noteOnFns;
    uint8_t activeNotes[128];
    uint8_t activeVoices[16];

    uint8_t getFirstFreeVoice() {
        for (int i=0; i < 16; i++) {
            if (activeVoices[i] == 255) {
                return i;
            }
        }
        return 255;
    }
};


#endif //TEENSYSEQUENCER_POLYPHONICSAMPLER_H