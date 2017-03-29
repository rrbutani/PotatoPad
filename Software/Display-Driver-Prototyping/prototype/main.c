#include <stdio.h>

#include "UART.h"
#include "PLL.h"
#include "graphics.h"

#include "common.h"

boolean setup(void)
{
  PLL_Init();
	Output_Init();
	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
	
	// Initialize PortF (F0 - F4; 0b00011111 -> 0x1F)
	GPIO_PORTF_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTF_CR_R    |=  0x1F;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTF_DIR_R   |=  0x1F;				  // Set pins as outputs
	GPIO_PORTF_DEN_R   |=  0x1F;					// Enable digital on pins
	GPIO_PORTF_AMSEL_R &= ~0x1F;					// Disable analog mode
	GPIO_PORTF_AFSEL_R &= ~0x1F;					// Disable alternate mode
	
	return true;
}

void blink(void)
{
	GPIO_PORTF_DATA_R = 0x08 & (~GPIO_PORTF_DATA_R);
}

void blinkr(void)
{
	GPIO_PORTF_DATA_R = 0x04 & (~GPIO_PORTF_DATA_R);
}

int main(void)
{
	setup();
	printf("hello\n");
	
	for(int a = 0; a < 100; a++)
	{
		printf("%d\n",a);
	}
	boolean a = initalizeGraphics();
	printf("1212");
	while(1){
		if(a)
			blink();
		else
			blinkr();
	}
}


