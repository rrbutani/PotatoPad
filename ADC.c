// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: Junlin Zhu and Rahul Butani
// Last modification date: April 5th, 2017

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ 
	SYSCTL_RCGCADC_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x08;
	while ((SYSCTL_PRGPIO_R & 0x08) == 0) {}
	GPIO_PORTD_DIR_R &= ~0x03;	// Set D0 and D2 to input
	GPIO_PORTD_DEN_R &= ~0x03;	// Disable digital
	GPIO_PORTD_AFSEL_R |= 0x03;		// Enable alternate function
	GPIO_PORTD_AMSEL_R |= 0x03;		// Enable analog function
	
	while ((SYSCTL_RCGCADC_R & 0x01) == 0x00) {}
  ADC0_PC_R = 0x01;            //	Set max speed to 125k Sample/s
  ADC0_SSPRI_R = 0x0123;        //	Set priority
  ADC0_ACTSS_R &= ~0x08;        // Disable Sequencer
  ADC0_EMUX_R &= ~0xF000;       // Set to software start
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R & ~0xFF) + 0x67;       // Use channel 6 and channel 7
	ADC0_SSCTL3_R = 0x06;         // TSO = 0 for  voltage, D0 = 0 to sample single
																//	IE0 = 1 to update flag, END0 = 1 for sample single
	//ADC0_SAC_R |= 0x04;						// Enable hardware averaging
  ADC0_IM_R &= ~0x08;         // Disable interrupt
  ADC0_ACTSS_R |= 0x08;       // Enable sequencer
}

//------------ADC_XIn------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: two 8-bit result of ADC conversion
void ADC_In(uint8_t data[2]){  
	ADC0_PSSI_R |= 0x08;
	while ((ADC0_RIS_R & 0x08) == 0) {}
	data[0] = (uint8_t)((ADC0_SSFIFO3_R & 0x0FF0) >> 4);
	data[1] = (uint8_t)((ADC0_SSFIFO3_R & 0x0FF0) >> 4);
	ADC0_ISC_R |= 0x08;
}
