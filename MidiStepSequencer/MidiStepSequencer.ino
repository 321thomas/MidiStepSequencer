/*
 Name:		MidiStepSequencer.ino
 Created:	12/19/2017 7:29:43 PM
 Author:	thomas
*/


#include "TM1637.h"
#define CLK 2//pins definitions for TM1637 and can be changed to other ports       
#define DIO 3
TM1637 tm1637(CLK, DIO);

//Mux control pins
int s0 = 8;
int s1 = 9;
int s2 = 10;
int s3 = 11;

//Mux in “SIG” pin
int SIG_pin = 0;

// the setup function runs once when you press reset or power the board
void setup() {
	tm1637.init();
	tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

	pinMode(LED_BUILTIN, OUTPUT);

	pinMode(s0, OUTPUT);
	pinMode(s1, OUTPUT);
	pinMode(s2, OUTPUT);
	pinMode(s3, OUTPUT);

	digitalWrite(s0, LOW);
	digitalWrite(s1, LOW);
	digitalWrite(s2, LOW);
	digitalWrite(s3, LOW);

	Serial.begin(9600);


}

// the loop function runs over and over again until power down or reset
void loop() {
	int8_t ListDisp[4];
	ListDisp[0] = 3;
	//Loop through and read all 16 values
	//Reports back Value at channel 6 is: 346
	for (int i = 0; i < 1; i++) {
		Serial.print("Value at channel ");
		Serial.print(i); Serial.print("is : ");
		int x = readMux(i);

		char buf[4];
		sprintf(buf, "%03i", x);

		int digit1, digit2, digit3, digit4;

		digit1 = buf[0] - '0';
		digit2 = buf[1] - '0';
		digit3 = buf[2] - '0';
		digit4 = buf[3] - '0';

		Serial.println(readMux(i));
		tm1637.display(0, digit1);
		tm1637.display(1, digit2);
		tm1637.display(2, digit3);
		tm1637.display(3, digit4);
		delay(10);
	}

}

int convert(int num)
{

}

int readMux(int channel) {
	int controlPin[] = { s0, s1, s2, s3 };
	int muxChannel[16][4] = { { 0,0,0,0 },
		//channel 0 
	{1,0,0,0},
		//channel 1 
	{0,1,0,0},
		//channel 2 
	{1,1,0,0},
		//channel 3 
	{0,0,1,0},
		//channel 4
	{1,0,1,0},
		//channel 5
	{0,1,1,0},
		//channel 6
	{1,1,1,0},
		//channel 7 
	{0,0,0,1},
		//channel 8
	{1,0,0,1},
		//channel 9 
	{0,1,0,1},
		//channel 10 
	{1,1,0,1},
		//channel 11 
	{0,0,1,1},
		//channel 12 
	{1,0,1,1},
		//channel 13 
	{0,1,1,1},
		//channel 14 
	{1,1,1,1}
		//channel 15 
	};
	//loop through the 4 sig 
	for (int i = 0; i < 4; i++) {
		digitalWrite(controlPin[i], muxChannel[channel][i]);
	} //read the value at the SIG pin 
	int val = analogRead(SIG_pin);
	//return the value 
	return val;
	//internalLedBlink(100, 100);

}

void internalLedBlink(int ms, int count)
{
	for (int i = 0; i < count; i++)
	{
		digitalWrite(LED_BUILTIN, HIGH);
		delay(ms);
		digitalWrite(LED_BUILTIN, LOW);
		delay(ms);
	}

}