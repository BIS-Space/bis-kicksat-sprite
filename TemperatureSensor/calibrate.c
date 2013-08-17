/*
	calibrate.c - uses the data points in the cc430f5137 TLV to find a linear
	relationship between ADC value and temperature in degrees C.
	Created by Laurence de Bruxelles, June 2013
	Released into the public domain
*/

#include "Energia.h"
#include "temp.h"

void setMainGainOffset(int8_t new_gain, int32_t new_offset)
{
	gain = new_gain;
	offset = new_offset;
}

void mainTempCalibrate(void)
{
	const uint16_t * temp35 = (const uint16_t *) 0x1a1a; // adc value at 35 degrees C
	const uint16_t * temp85 = (const uint16_t *) 0x1a1c; // adc value at 85 degrees C

	gain = 12800.0/(*temp85 - *temp35); // (2^8)*(85 - 35)/(temp85 - temp35)
	offset = (int32_t)(*temp35)*gain - 8960 + 128; // 2^8*c = 2^8*m*x + 2^8*8*y + 0.5*2^8
}
