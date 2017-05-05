// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: Junlin Zhu and Rahul Butani
// Last modification date: April 5th, 2017

#include <stdint.h>
#include "../contrib/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ 
	SYSCTL_RCGCADC_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x08;
	while ((SYSCTL_PRGPIO_R & 0x08) == 0) {}
	GPIO_PORTD_DIR_R &= ~0x0C;	// Set D2 and D3 to input
	GPIO_PORTD_DEN_R &= ~0x0C;	// Disable digital
	GPIO_PORTD_AFSEL_R |= 0x0C;		// Enable alternate function
	GPIO_PORTD_AMSEL_R |= 0x0C;		// Enable analog function
	
	while ((SYSCTL_RCGCADC_R & 0x01) == 0x00) {}
  ADC0_PC_R = (ADC0_PC_R & ~0xF) | 0x1;
  ADC0_SSPRI_R = 0x3210;          // Sequencer 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // use software trigger
  ADC0_SSMUX2_R = 0x0045;         // set channels
  ADC0_SSCTL2_R = 0x0060;         // TSO = 0 for  voltage, D0 = 0 to disable differential
																	// IE0 = 1 to update flag, END0 = 1 for sample single
  ADC0_IM_R &= ~0x0004;           // disable interrupts
  ADC0_ACTSS_R |= 0x0004;         // enable sequencer 2
}

//------------ADC_XIn------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: two 8-bit result of ADC conversion
void ADC_In(uint8_t data[2]){  
	ADC0_PSSI_R |= 0x04;
	while ((ADC0_RIS_R & 0x04) == 0) {}
	data[1] = (uint8_t)((ADC0_SSFIFO2_R & 0x0FF0) >> 4);
	data[0] = (uint8_t)((ADC0_SSFIFO2_R & 0x0FF0) >> 4);
	ADC0_ISC_R |= 0x04;
}
