// MidiSequencer.h

#ifndef _MIDISEQUENCER_h
#define _MIDISEQUENCER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

class MidiSequencerClass
{
 protected:


 public:
	void init();
};

extern MidiSequencerClass MidiSequencer;

#endif

