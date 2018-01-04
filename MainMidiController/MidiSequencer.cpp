// 
// 
// 

#include "MidiSequencer.h"
#define MIDISERIAL Serial1	//RX 0, TX 1 -> MIDI

MidiSequencer::MidiSequencer(int midiInPin, int midiOutPin){
    _midiInPin = midiInPin;
    _midiOutPin = midiOutPin;
    
}

void MidiSequencerClass::init()
{


}


MidiSequencerClass MidiSequencer;

