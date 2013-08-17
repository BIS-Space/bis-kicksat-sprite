#include "CCFlash.h"
#include "radtest.h"

unsigned int errors = 0;

void setup()
{
	erase_memory();

	Serial.begin(9600);
}

void loop()
{
	errors = check_memory();

	Serial.print("errors: ");
	Serial.println(errors);
}
