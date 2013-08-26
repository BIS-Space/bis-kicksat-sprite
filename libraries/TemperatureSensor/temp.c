/*
	temp.c - Library to read internal temperature sensor
	of cc430f5137 in the Kicksat Sprite. Based on Energia wiring_analog.c
	Written by Laurence de Bruxelles, June 2013
	Released into the public domain
*/

#include "Energia.h"
#include "temp.h"

#include "TI_USCI_I2C_master.h"

// need cc430f5137 with 12-bit ADC
#if defined(__MSP430_HAS_ADC12_PLUS__)

// defualt settings
int8_t gain = 40;
int32_t offset = 83591;
int32_t premult = 1;

// private functions
int16_t itg3200Read(void);
uint16_t cc430Read(void);

unsigned char rx_buf[1];
unsigned char tx_buf[2];

int32_t mainTempRead(void)
{
	int32_t temp = (premult*((uint32_t)cc430Read()*gain - offset)) >> 8;
	return temp;
}

int32_t gyroTempRead(void)
{
	int32_t temp = premult*35 + premult*((int32_t)itg3200Read()+13200)/280; // conversion according to datasheet
	return temp;
}

int16_t itg3200Read(void)
{
	tx_buf[0] = 0x1B; // First temperature register

	TI_USCI_I2C_transmitinit(GYRO_ADDRESS, I2C_PRESCALE);
	TI_USCI_I2C_transmit(tx_buf, 1);

	TI_USCI_I2C_receiveinit(GYRO_ADDRESS, I2C_PRESCALE);
	TI_USCI_I2C_receive(rx_buf, 2);

	int16_t temp = ((int)rx_buf[0]<<8) + (int)rx_buf[1];
	
	return temp;
}

uint16_t cc430Read(void)
{
	ADC12CTL0 &= ~ADC12ENC;
	ADC12CTL1 = ADC12SSEL_0 | ADC12DIV_4 | ADC12SHP; // clock settings
	ADC12CTL2 |= ADC12RES_2; // 12-bit resolution

	while(REFCTL0 & REFGENBUSY); // wait until free
	REFCTL0 |= REFON | REFVSEL_0; // use internal 1V5 ref
	ADC12MCTL0 = TEMPSENSOR | ADC12SREF_1; // set pin and ref
	ADC12CTL0 = ADC12ON | ADC12SHT0_4; // sample time 64 * clock

	// setup interrupts
	ADC12IFG = 0;
	ADC12IE |= ADC12IE0;

	__delay_cycles(128);
	ADC12CTL0 |= ADC12ENC | ADC12SC;  // start converting
	while (ADC12CTL1 & ADC12BUSY) {
		// wait in LPM0 until read & converted
		__bis_SR_register(CPUOFF + GIE);
	}

	// after return to active mode from ISR

	// power off modules
	ADC12CTL0 &= ~ADC12ON;
	REFCTL0 &= ~REFON;

	return ADC12MEM0;
}

// __attribute__((interrupt(ADC12_VECTOR)))
interrupt(ADC12_VECTOR) ADC12_ISR(void)
{
		switch(ADC12IV,36) {
		case 0: break; // No interrupt
		case 2: break; // conversion result overflow
		case 4: break; // conversion time overflow
		case 6:
				// reading ready, return to active mode
				 __bic_SR_register_on_exit(CPUOFF);
				 break; // Clear CPUOFF bit from 0(SR)                         
		default: break;
	}

	ADC12IFG = 0; // Clear Flags
}

#endif
