/*
 Name:		MainMidiController.ino
 Created:	12/26/2017 12:24:22 PM
 Author:	thoma
*/

//#include <MIDI.h>

//MIDI_CREATE_DEFAULT_INSTANCE();

#include "Bounce2.h"
#include "NoteToPitch.h"
#include "MidiHelper.h"
#include "MidiSequencer.h"
#include "WString.h"
#include <SPI.h>
//#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>

/* I/O */
// If using software SPI (the default case):
#define OLED_MOSI1   19  // D1
#define OLED_CLK1   20   // D0
#define OLED_DC1    22    // A0
#define OLED_CS1    21
#define OLED_RESET1 23

#define OLED_MOSI2   19  // D1
#define OLED_CLK2   20   // D0
#define OLED_DC2    17    // A0
#define OLED_CS2    16
#define OLED_RESET2 18

#define START_STOP 15
#define MODE_CHANGE 14
#define BASE_NOTE_CHANGE 37
#define PATTERN_LENGHT_CHANGE 38

Bounce btnStartStop = Bounce();

Adafruit_SSD1306 display1(OLED_MOSI1, OLED_CLK1, OLED_DC1, OLED_RESET1, OLED_CS1);
Adafruit_SSD1306 display2(OLED_MOSI2, OLED_CLK2, OLED_DC2, OLED_RESET2, OLED_CS2);

// Encoders
Encoder encBpm(33, 34);
Encoder encValue(35, 36);
int oldEncoderValues[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int newEncoderValues[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

/* I/O END*/

volatile boolean isRunning = false;
int ledPin = 13;
//IntervalTimer noteOnTimer;
static IntervalTimer noteOffTimer;
//IntervalTimer noteSetTimer;

static IntervalTimer stepTimer;
volatile prog_uint8_t currentStep = 0;
volatile prog_uint8_t lastNote = 0;
int bpm = 80;
float gate = 0.8;
bool bpmChanged = false;
int stepLengthMicroseconds = (60.0 / (bpm * 4)) * 1000 * 1000;
int base = C3;
bool transmode = true;


volatile int currentPattern = 0;
int patterncount = 4;

int transmodes[] = { 1,6,5,4 };
volatile int mode = 0; //0=all notes(1-12), 1=Ionian, 2=Dorian, 3=Phrygian, 4=Lydian, 5=Mixolydian, 6=Aeolian, 7=Locrian
int ionian[] = { 0,2,4,5,7,9,11,12 };
int dorian[] = { 0,2,3,5,7,9,10,12 };
int phygian[] = { 0,1,3,5,7,8,10,12 };
int lydian[] = { 0,2,4,6,7,9,11,12 };
int mixolydian[] = { 0,2,4,5,7,9,10,12 };
int aeolian[] = { 0,2,3,5,7,8,10,12 };
int locrian[] = { 0,1,3,5,6,8,10,12 };

String notes_s[12] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
String modes[7] = { "ionian", "dorian", "phrygian", "lydian", "mixolydian","aeolian","locrian" };
int intervals[7][7] = {
	{ 2,2,1,2,2,2,1 }, // ionian
	{ 2,1,2,2,2,1,2 }, // dorian
	{ 1,2,2,2,1,2,2 }, // phrygian
	{ 2,2,2,1,2,2,1 }, // lydian
	{ 2,2,1,2,2,1,2 }, // mixolydian
	{ 2,1,2,2,1,2,2 }, // aeolian
	{ 1,2,2,1,2,2,2 }  // locrian
};

// stufe = 0-6, note=1-12
int transpose(int mode, int stufe, int note, int baseNote) {
	note--; // reduce to start by 0

	int noteInMode = 0;
	for (int i = 0; i < note; i++)
	{
		noteInMode += intervals[mode][i];
	}

	for (int i = 0; i < (stufe - 1); i++)
	{
		int y = i + mode;
		if (y > 6) {
			y = y - 7;
		}
		noteInMode += intervals[y][note];

	}
	return noteInMode + baseNote;
}

int getNote(int mode, int note) {
	note--; // reduce to start by 0

	int noteInMode = 0;
	for (int i = 0; i < note; i++)
	{
		noteInMode += intervals[mode][i];
	}

	return noteInMode;
}

String convertToNote(int note)
{
	if (note > 11) {
		note = note - 12;
	}
	return notes_s[note];
}



double interval = 30 * (double)1000 * 1000;
volatile double noteLength = 10;
volatile int note = 100;
//volatile int lastNote = 100;

int steps = 64;
//volatile int currentStep = 1;
// 60 => C4, 63 => Eb, 67 => G, 72 => C5
//int notes[] = { C4,E4b,G4,C4,C5,C4,C5,G4,C4,G4,E4b,C5,G4,C4,C5,G4,C4,E4b,G4,C4,C5,C4,C5,G4,C4,G4,E4b,C5,G4,C4,C5,G4,C4,E4b,G4,C4,C5,C4,C5,G4,C4,G4,E4b,C5,G4,C4,C5,G4,C4,E4b,G4,C4,C5,C4,C5,G4,C4,G4,E4b,C5,G4,C4,C5,G4 };
int notes[] = { 1,3,5,3,6,5,3,5,1,3,5,3,6,5,3,5 };

#define MIDISERIAL Serial1	//RX 0, TX 1 -> MIDI
#define HWSERIAL Serial2	// RX 9, TX 10 -> Arduino Link


// the setup function runs once when you press reset or power the board
void setup() {
	/*pinMode(2, INPUT);
	pinMode(3, OUTPUT);
*/
	pinMode(START_STOP, INPUT_PULLDOWN); // start/stop
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

	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display1.begin(SSD1306_SWITCHCAPVCC);
	display2.begin(SSD1306_SWITCHCAPVCC);

	display1.clearDisplay();
	display1.setTextColor(WHITE);
	display1.setCursor(0, 0);
	display1.setTextSize(2);
	display1.println("Starting...");
	display1.display();

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
	mode = 0;// transmodes[currentPattern];
	int note = notes[currentStep] - 1;



	note = transpose(mode, transmodes[currentPattern], note + 1, 0);
	Serial.println(convertToNote(note));
	//Serial.print("Mode: ");
	//Serial.println(mode);
	/*switch (mode)
	{
	case 1:
		note = base + ionian[note];
		break;
	case 2:
		note = base + dorian[note];
		break;
	case 3:
		note = base + phygian[note];
		break;
	case 4:
		note = base + lydian[note];
		break;
	case 5:
		note = base + mixolydian[note];
		break;
	case 6:
		note = base + aeolian[note];
		break;
	case 7:
		note = base + locrian[note];
		break;
	default:
		break;
	}*/
	note = note + base;
	noteOn(note, 127, 100);
	lastNote = note;
	noteOffTimer.end();
	noteOffTimer.begin(noteOff, (stepLengthMicroseconds*gate) - 10);
	currentStep++;

	//Serial.println(currentStep);


	if (currentStep % 16 == 0) {
		currentPattern++;
		if (transmode) {
			//Serial.println("Reset");
			currentStep = 0;
		}
	}

	if (currentPattern == patterncount)
	{
		currentPattern = 0;
	}

	if (currentStep == steps) {
		currentStep = 0;
		currentPattern = 0;
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
	unsigned long timeout = start + 100;
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