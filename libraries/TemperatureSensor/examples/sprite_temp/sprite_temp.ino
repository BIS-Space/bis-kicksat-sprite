/*
	Demonstrates use of temperature library.
	Here we print the temperature over serial
	with the Sprite developer kit
*/

#include <temp.h>

int32_t temp;

void setup()
{
	Serial.begin(9600);

	premult = 10;

	mainTempCalibrate();
	mainTempRead(); // first reading is usually spurious
}

void loop()
{
	temp = mainTempRead();
	Serial.print("main: ");
	Serial.print(temp / 10);
	Serial.print('.');
	Serial.print(temp % 10);
	Serial.print(" degrees, ");

	temp = gyroTempRead();
	Serial.print("gyro: ");
	Serial.print(temp / 10);
	Serial.print('.');
	Serial.print(temp % 10);
	Serial.println(" degrees");

	delay(1000);
}
