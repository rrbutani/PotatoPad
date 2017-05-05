// input.c
// Runs on LM4F120/TM4C123
// input module for PotatoDoom
// Reads data from joysticks and buttons
// Steven Zhu

#include <stdint.h>

#include "contrib/tm4c123gh6pm.h"
#include "drivers/ADC.h"
#include "common.h"
#include "audio.h"
#include "input.h"

static boolean disabled;

void inputInit(void) {
	ADC_Init();
	
	// Port F init
	SYSCTL_RCGCGPIO_R |= 0x20;
	while ((SYSCTL_PRGPIO_R & 0x20) == 0) {}
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // Unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // Allow changes to PF0
	GPIO_PORTF_DIR_R = (GPIO_PORTF_DIR_R & 0x1D) | 0x02;
	GPIO_PORTF_PUR_R |= 0x1D;					// Negative logic
	GPIO_PORTF_DEN_R |= 0x1F;
	GPIO_PORTF_AMSEL_R &= ~0x1F;			// Disable analog functions
	GPIO_PORTF_AFSEL_R &= ~0x1F;			// Disable alternate functions
	GPIO_PORTF_PCTL_R &= ~0x1F;	
		
	GPIO_PORTF_IM_R = 0;
	GPIO_PORTF_IS_R &= ~0x1D;     //  Edge-sensitive
  GPIO_PORTF_IBE_R = (GPIO_PORTF_IBE_R & ~0x1C) | 0x01;    //  Only one edge for F2,F3,F4 both edge for F0
  GPIO_PORTF_IEV_R &= ~0x1C;		//	Only falling edge
	GPIO_PORTF_ICR_R = 0x1D;    // Reset interrupt
  GPIO_PORTF_IM_R |= 0x1D;		// Arm interrupt
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00600000; // Set priority to 3
	NVIC_EN0_R = 1 << 30;
		
	disabled = true;
}

void updateSpeed(void) {
	uint8_t data[2];
	ADC_In(data);
	if (data[0] > deadZoneStart && data[0] < deadZoneEnd)
		player.speed = 0;
	else
		player.speed = (float)(128-data[0]) * maxSpeed / 128;
	if (data[1] > deadZoneStart && data[1] < deadZoneEnd)
		player.angularSpeed = 0;
	else
		player.angularSpeed = (float)(128-data[1]) * maxAngularSpeed / 128;
}

void disableInput(void) {
	disabled = true;
}

void enableInput(void) {
	disabled = false;
}

void GPIOPortF_Handler(void) {
	uint8_t triggered = GPIO_PORTF_RIS_R & 0x1D;
	GPIO_PORTF_ICR_R = triggered;	// Acknowledge interrupt
	if (disabled)	return;
	if (triggered & 0x01) {
		if (GPIO_PORTF_DATA_R & 0x01)
			player.running = 1;
		else
			player.running = 2;
	} 
	if (triggered & 0x04) {
	}
	if (triggered & 0x08) {
		player.shooting = true;
		playSFX(pistol);
	}
	if (triggered & 0x10) {
		toggleMusic();
	}
}
