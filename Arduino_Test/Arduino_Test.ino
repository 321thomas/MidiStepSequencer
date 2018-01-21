#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "WString.h"

// If using software SPI (the default case):
#define OLED_MOSI   10
#define OLED_CLK   11
#define OLED_DC    3
#define OLED_CS    4
#define OLED_RESET 2
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 


int latchPin = 8;
int clockPin = 9;
int dataPin = 7;

int numOfRegisters = 2;
byte* registerState;



#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() {
	Serial.begin(9600);

	//Initialize array
	registerState = new byte[numOfRegisters];
	for (size_t i = 0; i < numOfRegisters; i++) {
		registerState[i] = 0;
	}

	//set pins to output so you can control the shift register
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);

	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display.begin(SSD1306_SWITCHCAPVCC);
	// init done

	
}


void loop() {
	// text display tests
	for (size_t i = 0; i < 16; i++)
	{
		regWrite(i, HIGH);


		display.clearDisplay();
		//display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(0, 0);
		//display.println("Hello, world!");

		//display.setTextColor(8);
		display.setTextSize(3);
		display.println("120 BPM");
		display.setTextSize(1);

		display.print("1/" + ((String)(i + 1)));
		display.println("   1/4  IIV");
		display.println("ioanian C3");
		display.println("Pattern 21");
		//display.setTextColor(BLACK, WHITE); // 'inverted' text
		//display.println(3.141592);
		//display.setTextSize(2);
		//display.setTextColor(WHITE);
		//display.print("0x"); display.println(0xDEADBEEF, HEX);
		display.display();
		delay(300);
		regWrite(i, LOW);
		//		//delay(100);

	}


	display.clearDisplay();

}







//
//void loop() {
//	for (size_t i = 0; i < 16; i++)
//	{
//		regWrite(i, HIGH);
//		delay(60);
//		regWrite(i, LOW);
//		//delay(100);
//	}
//	for (size_t i = 14; i > 0; i--)
//	{
//		regWrite(i, HIGH);
//		delay(60);
//		regWrite(i, LOW);
//		//delay(100);
//	}
//	/*regWrite(0, HIGH);
//	delay(200);
//	regWrite(0, LOW);
//	delay(200);*/
//
//
//}
//
//
//
//
void regWrite(int pin, bool state) {
	//Determines register
	int reg = pin / 8;
	//Determines pin for actual register
	int actualPin = pin - (8 * reg);

	//Begin session
	digitalWrite(latchPin, LOW);


	for (int i = 0; i < numOfRegisters; i++) {
		//Get actual states for register
		byte* states = &registerState[i];

		//Update state
		if (i == reg) {
			bitWrite(*states, actualPin, state);
		}

		//Write
		shiftOut(dataPin, clockPin, MSBFIRST, *states);
	}

	//End session
	digitalWrite(latchPin, HIGH);
	;
}