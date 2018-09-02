// 
// 
// 

#include "MidiSequencer.h"
#define MIDISERIAL Serial1	//RX 0, TX 1 -> MIDI

int _midiInPin;
int _midiOutPin;

MidiSequencer::MidiSequencer(int midiInPin, int midiOutPin){
    _midiInPin = midiInPin;
    _midiOutPin = midiOutPin;
    
}






