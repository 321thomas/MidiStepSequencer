/*
 Name:		MidiController.ino
 Created:	12/27/2017 6:37:05 PM
 Author:	thomas
*/

#include <SoftwareSerial.h>
#include "TM1637.h"
#define CLK 5//pins definitions for TM1637 and can be changed to other ports       
#define DIO 4
int BPM_INPUT = 3;
TM1637 tm1637(CLK, DIO);

SoftwareSerial mySerial(10, 11); // RX, TX


//Mux in “SIG” pin
uint16_t volumeSIG_pin = 0;

uint16_t bpm = 0;
uint16_t bpmRaw = 0;
uint16_t volumeRawValues[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
uint16_t volumeValues[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
uint16_t volumeControlPins[] = { 6, 7, 8, 9 };
uint16_t lastValueToDisplay = 0;

uint16_t muxChannel[16][4] = { { 0,0,0,0 },
//channel 0 
{ 1,0,0,0 },
//channel 1 
{ 0,1,0,0 },
//channel 2 
{ 1,1,0,0 },
//channel 3 
{ 0,0,1,0 },
//channel 4
{ 1,0,1,0 },
//channel 5
{ 0,1,1,0 },
//channel 6
{ 1,1,1,0 },
//channel 7 
{ 0,0,0,1 },
//channel 8
{ 1,0,0,1 },
//channel 9 
{ 0,1,0,1 },
//channel 10 
{ 1,1,0,1 },
//channel 11 
{ 0,0,1,1 },
//channel 12 
{ 1,0,1,1 },
//channel 13 
{ 0,1,1,1 },
//channel 14 
{ 1,1,1,1 }
//channel 15 
};

// the setup function runs once when you press reset or power the board
void setup() {

	mySerial.begin(115200);
	tm1637.init();
	tm1637.set(BRIGHTEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

	for (uint8_t i = 0; i < 4; i++)
	{
		pinMode(volumeControlPins[i], OUTPUT);
		digitalWrite(volumeControlPins[i], LOW);
	}

	Serial.begin(115200);

	pinMode(LED_BUILTIN, OUTPUT);

	attachInterrupt(0, sendData, RISING);
}

// the loop function runs over and over again until power down or reset
void loop() {

	//Loop through and read all 16 values
	//Reports back Value at channel 6 is: 346
	unsigned long start = micros();
	readMuxToArray(volumeControlPins, volumeSIG_pin, volumeRawValues, volumeValues, 3, 0, 200);
	readBpm();
	delay(5);

	// display last changed value
	char buf[4];
	sprintf(buf, "%04i", lastValueToDisplay);

	uint16_t digit1, digit2, digit3, digit4;

	digit1 = buf[0] - '0';
	digit2 = buf[1] - '0';
	digit3 = buf[2] - '0';
	digit4 = buf[3] - '0';

	tm1637.display(0, digit1);
	tm1637.display(1, digit2);
	tm1637.display(2, digit3);
	tm1637.display(3, digit4);

	unsigned long end = micros();
	//Serial.println(volumeValues[0]);
	//Serial.print("Dauer: ");
	//Serial.println(end - start);

	delay(100);
}

void readBpm() {
	//Serial.println(analogRead(BPM_INPUT));
	uint16_t value = analogRead(BPM_INPUT);

	int diff = value - bpmRaw;
	if (diff < 0) {
		diff = diff * -1;
	}
	//Serial.println(diff);
	if (diff < 5) {
		bpmRaw = (bpmRaw * 14) + (value * 2);
		bpmRaw = (bpmRaw + 8) >> 4;
	}
	else {
		bpmRaw = value;
	}
	uint16_t newBpm = map(bpmRaw, 0, 1023, 12, 180);
	if (newBpm != bpm) {
		lastValueToDisplay = newBpm;
		bpm = newBpm;
	}
	//Serial.println(bpm);
}

void readMuxToArray(uint16_t controlPins[], uint16_t SIG_pin, uint16_t rawValues[], uint16_t values[], uint16_t valueCount, uint16_t mapMinValue, uint16_t mapMaxValue)
{
	uint8_t threshold = 10;
	// read the first 4 channels
	for (uint8_t i = 0; i < valueCount; i++)
	{
		uint16_t oldValue = values[i];
		uint16_t currentValue = readMux(i, SIG_pin, controlPins);

		int diff = currentValue - rawValues[i];
		if (diff < 0) {
			diff = diff * -1;
		}

		if (diff < threshold) {
			rawValues[i] = (rawValues[i] * 14) + (currentValue * 2);
			rawValues[i] = (rawValues[i] + 8) >> 4;
		}
		else {
			rawValues[i] = currentValue;
			//Serial.println("QUICK CHANGE");
		}
		if (mapMaxValue > 0) {
			values[i] = map(rawValues[i], 0, 1023, mapMinValue, mapMaxValue);
		}
		if (oldValue != values[i]) {
			lastValueToDisplay = values[i];
		}
	}
}

uint16_t readMux(uint16_t channel, uint16_t SIG_pin, uint16_t controlPin[]) {
	//loop through the 4 sig 
	for (uint8_t i = 0; i < 4; i++) {
		digitalWrite(controlPin[i], muxChannel[channel][i]);
	} //read the value at the SIG pin 
	return analogRead(SIG_pin);
}

void sendData() {
	mySerial.write(lowByte(bpm));
	mySerial.write(highByte(bpm));
	for (uint8_t i = 0; i < (sizeof(volumeValues) / sizeof(uint16_t)); i++)
	{
		mySerial.write(lowByte(volumeValues[i]));
		mySerial.write(highByte(volumeValues[i]));
	}
}
