/*
 Name:		Teensy_Test.ino
 Created:	1/21/2018 10:55:14 AM
 Author:	thoma
*/


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "WString.h"
#include <Encoder.h>

// If using software SPI (the default case):
#define OLED_MOSI1   20  // D1
#define OLED_CLK1   21   // D0
#define OLED_DC1    18    // A0
#define OLED_CS1    17
#define OLED_RESET1 19

#define OLED_MOSI2   20  // D1
#define OLED_CLK2   21  // D0
#define OLED_DC2    23    // A0
#define OLED_CS2    22
#define OLED_RESET2 19

Adafruit_SSD1306 display2(OLED_MOSI1, OLED_CLK1, OLED_DC1, OLED_RESET1, OLED_CS1);
Adafruit_SSD1306 display1(OLED_MOSI2, OLED_CLK2, OLED_DC2, OLED_RESET2, OLED_CS2);

// Encoders
Encoder encBpm(33, 34);
Encoder encValue(35, 36);



int oldPosBpm = -999;
int oldPosValue = -999;

int valBpm = -999;
int valValue = -999;


int currentBpm = 120;


int ledPin = 13;

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);


	Serial.begin(9600);

	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display2.begin(SSD1306_SWITCHCAPVCC);
	display1.begin(SSD1306_SWITCHCAPVCC, 0, false);
}

long t1;
// the loop function runs over and over again until power down or reset
void loop() {
	long newPosPbm = encBpm.read();
	long newPos2Value = encValue.read();
	
	if (newPosPbm != oldPosBpm || true)
	{
		valBpm = valBpm + (newPosPbm - oldPosBpm);
		if (valBpm >= 0) {
			//long t1; = map(valBpm, 0, 800, 0, 200);

			t1++;
			oldPosBpm = valBpm;
			display1.clearDisplay();
			display1.setTextColor(WHITE);
			display1.setCursor(0, 0);

			display1.setTextSize(3);

			display1.println((String)t1 + " BPM");
			display1.setTextSize(1);

			display1.print("1/4");// " + ((String)(i + 1)));
			display1.println("   1/4  IIV");
			display1.println("ioanian C3");
			display1.println("Pattern 21");
			display1.display();


			display2.clearDisplay();
			display2.setTextColor(WHITE);
			display2.setCursor(0, 0);
			display2.setTextSize(3);
			display2.println("TEST");
			display2.display();

		}
		else {
			valBpm = 0;
			oldPosBpm = 0;
			encBpm.write(0);
		}
	}
	if (newPos2Value != oldPosValue)
	{
		//valValue = valValue + (newPos2Value - oldPosValue);
		//if (valValue >= 0) {
			/*oldPosValue = newPos2Value;
			display2.clearDisplay();
			display2.setTextColor(WHITE);
			display2.setCursor(0, 0);
			display2.setTextSize(3);
			display2.println(valValue);
			display2.display();*/
		/*}
		else {
			valValue = 0;
			encValue.write(0);
			oldPosValue = 0;
		}*/
	}




	delay(80);

	//display.clearDisplay();
}
