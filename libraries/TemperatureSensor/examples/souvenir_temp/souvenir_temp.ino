/*
	Demonstrates use of temperature library.
	Here we show the temperature in a range
	between TEMP_LO and TEMP_HI on the Sprite
	souvenir LEDs
*/

#include <temp.h>

#define TEMP_LO 10
#define TEMP_HI 20
#define TEMP_INC (TEMP_HI - TEMP_LO)/5

void setup()
{
	pinMode(6, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(2, OUTPUT);
	pinMode(1, OUTPUT);

	mainTempCalibrate();

	mainTempRead(); // first reading is usually spurious
}

void loop()
{
	showTemp(mainTempRead());

	delay(1000);
}

void showTemp(int16_t temp)
{
	// display the temperature on an LED scale
	
	digitalWrite(6,LOW);
	digitalWrite(5,LOW);
	digitalWrite(4,LOW);
	digitalWrite(3,LOW);
	digitalWrite(2,LOW);
	digitalWrite(1,LOW);

	if (temp >= TEMP_LO + 0*TEMP_INC) digitalWrite(6, HIGH);
	if (temp >= TEMP_LO + 1*TEMP_INC) digitalWrite(5, HIGH);
	if (temp >= TEMP_LO + 2*TEMP_INC) digitalWrite(4, HIGH);
	if (temp >= TEMP_LO + 3*TEMP_INC) digitalWrite(3, HIGH);
	if (temp >= TEMP_LO + 4*TEMP_INC) digitalWrite(2, HIGH);

	// out of bounds
	if ((temp < TEMP_LO) || (TEMP_HI <= temp)) digitalWrite(1, HIGH);
}