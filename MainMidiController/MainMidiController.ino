/*
 Name:		MainMidiController.ino
 Created:	12/26/2017 12:24:22 PM
 Author:	thoma
*/

//#include <MIDI.h>

//MIDI_CREATE_DEFAULT_INSTANCE();

#include "NoteToPitch.h"
#include "MidiHelper.h"
#include "MidiSequencer.h"
volatile boolean isRunning = false;
int ledPin = 13;
//IntervalTimer noteOnTimer;
static IntervalTimer noteOffTimer;
//IntervalTimer noteSetTimer;

static IntervalTimer stepTimer;
volatile prog_uint8_t currentStep = 0;
volatile prog_uint8_t lastNote = 0;
int bpm = 80;
bool bpmChanged = false;
int stepLengthMicroseconds = (60.0 / (bpm * 4)) * 1000 * 1000;



double interval = 30 * (double)1000 * 1000;
volatile double noteLength = 10;
volatile int note = 100;
//volatile int lastNote = 100;

int steps = 16;
//volatile int currentStep = 1;
// 60 => C4, 63 => Eb, 67 => G, 72 => C5
int notes[] = { C4,E4b,G4,C4,C5,C4,C5,G4,C4,G4,E4b,C5,G4,C4,C5,G4 };

#define MIDISERIAL Serial1	//RX 0, TX 1 -> MIDI
#define HWSERIAL Serial2	// RX 9, TX 10 -> Arduino Link


// the setup function runs once when you press reset or power the board
void setup() {
	/*pinMode(2, INPUT);
	pinMode(3, OUTPUT);
*/
	pinMode(2, INPUT_PULLDOWN); // start/stop
	attachInterrupt(2, start, FALLING);


	pinMode(11, OUTPUT);
	digitalWrite(11, LOW);

	//MIDI.begin();

	pinMode(4, INPUT_PULLDOWN);

	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);

	Serial.begin(115200);
	MIDISERIAL.begin(31250);
	HWSERIAL.begin(115200);
	Serial.println("Starting...");
	//unsigned long t1 = millis();

}

unsigned long count = 0;
// the loop function runs over and over again until power down or reset
void loop() {
	//unsigned long start = millis();
	readValues();
	//unsigned long end = millis();
	//Serial.println(count);
	count++;
	delay(100);
	//char  Buffer[768];



	//int newBpm = getBpm();
	////Serial.println(newBpm);
	//if (newBpm != bpm) {
	//	bpm = newBpm;
	//	noteLength = ((60.0 / (bpm * 4) * 1000) - 10) * 1000;
	//	interval = bpmToMicroseconds(bpm);
	//	noteOff();
	//	noteOnTimer.begin(bpmFunction, interval);
	//}
	//delay(5000);
}

void start() {
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 200)
	{
		if (isRunning) {
			stepTimer.end();
		}
		else {
			stepTimer.begin(nextStep, stepLengthMicroseconds);
		}
		isRunning = !isRunning;
		last_interrupt_time = interrupt_time;
	}

}

void setBpm(prog_uint8_t newBpm) {
	static unsigned long last_time_setBpm = 0;
	unsigned long set_time = millis();
	if (set_time - last_time_setBpm > 1)
	{
		if (newBpm != bpm) {
			bpm = newBpm;
			stepLengthMicroseconds = (60.0 / (bpm * 4)) * 1000 * 1000;
			bpmChanged = true;
			/*if (isRunning) {
				stepTimer.begin(nextStep, stepLengthMicroseconds);
			}*/
		}
		last_time_setBpm = set_time;
	}

}

void sendMidi(int cmd, int pitch, int velocity) {
	//unsigned long time = micros();
	MIDISERIAL.write(cmd);
	MIDISERIAL.write(pitch);
	MIDISERIAL.write(velocity);
	/*noteLength = (60.0 / (bpm * 4)) * 1000 * 1000;
	unsigned long timepassed = micros() - time;
	if (timepassed > 100000) {
		timepassed = 100;
	}
	noteLength = noteLength - (timepassed + 900);*/
}

void nextStep() {
	//noteOff(lastNote);

	int note = notes[currentStep];
	noteOn(note, 127, 100);
	lastNote = note;
	noteOffTimer.end();
	noteOffTimer.begin(noteOff, stepLengthMicroseconds - 100);
	//delayMicroseconds(stepLengthMicroseconds);
	//noteOff(note);
	currentStep++;
	if (currentStep == 16) {
		currentStep = 0;
	}
	if (bpmChanged) {
		bpmChanged = false;
		//stepTimer.end();
		stepTimer.begin(nextStep, stepLengthMicroseconds);
	}
}

void noteOn(int pitch, int velocity, int lengthMs)
{
	sendMidi(0x90, pitch, velocity);
	//IntervalTimer noteOffTimer;
}

void noteOff() {
	noteOff(lastNote);
	noteOffTimer.end();
}

void noteOff(int pitch)
{
	sendMidi(0x80, pitch, 0);
}

double bpmToMicroseconds(int bpm)
{
	return (60 / (double(bpm) * 4)) * 1000 * 1000;
}


void readValues() {
	//long starttime = millis();
	//int values[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	digitalWrite(11, HIGH);
	digitalWrite(11, LOW);

	int count = 0;
	long start = millis();
	unsigned long timeout = start + 800;
	bool done = false;

	while (done == false && timeout > millis()) {
		if (HWSERIAL.available() == 34) {
			byte buffer[34];
			HWSERIAL.readBytes(buffer, 34);

			byte lowByte = buffer[0];
			byte highByte = buffer[1];
			setBpm(highByte * 256 + lowByte);
			count++;
			for (int i = 2; i < 34; i = i + 2)
			{
				lowByte = buffer[i];
				highByte = buffer[i + 1];
				//values[count] = highByte * 256 + lowByte;
				count++;
			}
		}
		done = count == 17;
	}

	//long endtime = millis();
	if (done) {
		/*for (size_t i = 0; i < 16; i++)
		{
			Serial.println(values[i]);
		}*/
	}
	else
	{
		Serial.println("Timeout!");
	}

	//Serial.print("Duration [ms]:\t");
	//Serial.println(endtime - starttime);
	HWSERIAL.clear();
	delay(100);
}