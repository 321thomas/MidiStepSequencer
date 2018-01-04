// MidiSequencer.h

#ifndef _MIDISEQUENCER_h
#define _MIDISEQUENCER_h

/*
#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif
*/

class MidiSequencerClass
{
 protected:
private:
//int _midiInPin;
//int _midiOutPin;
int _bpm;
int _notes[64];
int _volumes[64];

 public:
 MidiSequencer();
 void start();
 void stop();
 void setBpm(int bpm);
 int *getVolumes(int values[64]);
 int *getNotes()int notes[64]);
 void setVolumes(int values[64]);
 void setNotes(int notes[64]);
	//void init();
};

extern MidiSequencerClass MidiSequencer;

#endif

