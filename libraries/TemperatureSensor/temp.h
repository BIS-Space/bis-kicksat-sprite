/*
	temp.h - Library to read internal temperature sensor
	of cc430f5137 in the Kicksat Sprite. Based on Energia wiring_analog.c
	Written by Laurence de Bruxelles, June 2013
	Released into the public domain
*/

#ifndef temp_h
#define temp_h

#include "Energia.h"

// need cc430f5137 with 12-bit ADC
#if defined(__MSP430_HAS_ADC12_PLUS__)

#define GYRO_ADDRESS 0x68 // for temperature sensor in ITG-3200

#ifdef __cplusplus
extern "C" {
#endif

// global variables
extern int8_t gain;
extern int32_t offset;
extern int32_t premult;

// visible interface
int32_t mainTempRead(void);
int32_t gyroTempRead(void);
void mainTempCalibrate(void);
void setMainGainOffset(int8_t new_gain,int32_t new_offset);

#ifdef __cplusplus
}
#endif

#endif
#endif
