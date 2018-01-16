int latchPin = 8;
int clockPin = 9;
int dataPin = 7;

int numOfRegisters = 2;
byte* registerState;


void setup() {
	//Initialize array
	registerState = new byte[numOfRegisters];
	for (size_t i = 0; i < numOfRegisters; i++) {
		registerState[i] = 0;
	}

	//set pins to output so you can control the shift register
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
}

void loop() {
	for (size_t i = 0; i < 8; i++)
	{
		regWrite(i, HIGH);
		delay(60);
		regWrite(i, LOW);
		//delay(100);
	}
	for (size_t i = 6; i > 0; i--)
	{
		regWrite(i, HIGH);
		delay(60);
		regWrite(i, LOW);
		//delay(100);
	}
	/*regWrite(0, HIGH);
	delay(200);
	regWrite(0, LOW);
	delay(200);*/


}




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