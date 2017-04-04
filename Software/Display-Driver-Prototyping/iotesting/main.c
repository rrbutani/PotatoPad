#include "tm4c123gh6pm.h"

static void initIO(void)
{
	// Start clock for Ports A, C, E, and F (0b00110101 -> 0x35)
	SYSCTL_RCGC2_R |= (SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOE | SYSCTL_RCGC2_GPIOF); 
	//while ((SYSCTL_RCGC2_R & 0x35) != 0x35) {}		// Wait a few cycles
		
	// Initialize PortA (A2 - A6; 0b01111100 -> 0x7C)
	GPIO_PORTA_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTA_CR_R    |=  0x7C;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTA_DIR_R   |=  0x7C;				  // Set pins as outputs
	GPIO_PORTA_DEN_R   |=  0x7C;					// Enable digital on pins
	GPIO_PORTA_AMSEL_R &= ~0x7C;					// Disable analog mode
	GPIO_PORTA_AFSEL_R &= ~0x7C;					// Disable alternate mode
	//GPIO_PORTA_PCTL_R  &= ~0x7C;					// Disable peripherals on port/clear port control
	// ^The above line breaks UART. Do not uncomment.
		
	// Initialize PortC (C4 - C7; 0b11110000 -> 0xF0)
	GPIO_PORTC_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTC_CR_R    |=  0xF0;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTC_DIR_R   |=  0xF0;				  // Set pins as outputs
	GPIO_PORTC_DEN_R   |=  0xF0;					// Enable digital on pins
	GPIO_PORTC_AMSEL_R &= ~0xF0;					// Disable analog mode
	GPIO_PORTC_AFSEL_R &= ~0xF0;					// Disable alternate mode
	//GPIO_PORTC_PCTL_R  &= ~0xF0;					// Disable peripherals on port/clear port control
	// ^The above line breaks the board (requires an LM Flash wipe to fix). Do not uncomment.	
		
	// Initialize PortE (E0 - E5; 0b00111111 -> 0x3F)
	GPIO_PORTE_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTE_CR_R    |=  0x3F;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTE_DIR_R   |=  0x3F;				  // Set pins as outputs
	GPIO_PORTE_DEN_R   |=  0x3F;					// Enable digital on pins
	GPIO_PORTE_AMSEL_R &= ~0x3F;					// Disable analog mode
	GPIO_PORTE_AFSEL_R &= ~0x3F;					// Disable alternate mode
	//GPIO_PORTE_PCTL_R  &= ~0x3F;					// Disable peripherals on port/clear port control
	// Disabling PCTL seems like a bad idea in general.

	// Initialize PortF (F0 - F4; 0b00011111 -> 0x1F)
	GPIO_PORTF_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTF_CR_R    |=  0x1F;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTF_DIR_R   |=  0x1F;				  // Set pins as outputs
	GPIO_PORTF_DEN_R   |=  0x1F;					// Enable digital on pins
	GPIO_PORTF_AMSEL_R &= ~0x1F;					// Disable analog mode
	GPIO_PORTF_AFSEL_R &= ~0x1F;					// Disable alternate mode
	//GPIO_PORTF_PCTL_R  &= ~0x1F;					// Disable peripherals on port/clear port control
	// Disabling PCTL seems like a bad idea in general.

	// Clear bus pins:
	GPIO_PORTA_DATA_R  &= ~0x7C;
	GPIO_PORTE_DATA_R  &= ~0x3F;
	GPIO_PORTF_DATA_R  &= ~0x1F;
	
	// Set CS, D/C, WR, RD pins all to high:
	GPIO_PORTC_DATA_R  |=  0xF0;
}

inline static void testUnfriendly(void)
{
	GPIO_PORTC_DATA_R = 0x00; // Attempt unfriendly access on Port C (and see if anything breaks).
	GPIO_PORTC_DATA_R = 0xFF;
	GPIO_PORTF_DATA_R = 0x00;
	GPIO_PORTC_DATA_R = 0xFF;
	GPIO_PORTA_DATA_R = 0x00;
	GPIO_PORTC_DATA_R = 0xFF;
	GPIO_PORTE_DATA_R = 0x00;
	GPIO_PORTC_DATA_R = 0xFF;
	
	//PORTA:0x40004000 << 6pins
	//PORTB:0x40005000 << 8pins
	//PORTC:0x40006000 << 4pins
	//PORTD:0x40007000 << 6pins
	//PORTE:0x40024000 << 6pins
	//PORTF:0x40025000 << 5pins
	
	// Likely going to use PortC for control (4 pins) and Ports A (6 pins), E (6 pins), and B (8 pins)
	// not using PortD because of the wacky bit shifting that would be needed (bits 4 and 5 of D go to USB)
	
	// Ideally we'd use 3 ports that start at the 0th bit and have 6 consecutive bits
	// Since we don't have 3 ports that match the above traits, we'll settle for PortA (consecutive but not starting at 0)
	
	// The order of the ports actually doesn't matter.
	// For rgb data dumping, each port takes 6 bits, so it doesn't matter here.
	// For sending commands, the first 8 bits are used which makes it seem like it would save time to use a port which 
	// 		has it's first bit at 0, since that would save us a bit shift, we since command values are fixed, the bit
	//		shifting can (and should) be done at compile time. So it doesn't matter here either.
	// For windowing/address setting (the data part of this - for the command part the above applies), the data isn't fixed.
	//		Here, 8 bits are used again (regardless of controller - applies to SSD1298 and ILI9341) and we're already going 
	//		to have to do bit-wrangling to split the 8 bit value into two of our 6 bit ports. Thus it might seem like it would
	//		save time to have the two ports used for the 8 bits to ports w/0 as the first bit (therefore no extra left shifting)
	//		But, this can again be accounted for at compile time. Additionally, in 8080-II mode, the first command bit actually
	//		goes on DB1 instead of DB0, meaning shifting is required anyways.
	//		As such, for this situation the order of the ports doesn't matter.
	// All other situations are not common enough to bother optimizing for or are otherwise constrained (i.e. reading from the bus)
	
	// With that in mind, here's the pinout
	// DB00:05 :: A2:7
	// DB06:11 :: B0:5
	// DB12:17 :: E0:5
	// Control :: C4:7
	
	// Optimal Situation:
	// Red, Green, and Blue are in R1, R2, R3 respectively
  // R3 (blue) is properly shifted already
	// R0 has an address
	
  // Using R1-3 as the port mask and operating on the base port memory addresses
  // is clever but actually less efficient (because we still would need to shift
  // R3 (blue -> PortA) separately because it needs to be shifted 4 to the left 
  // to be a port mask and max LSL for an STR is 3. Also we'd need a dummy register
  // with bits 0:7 set and none of the memory addresses fit this requirement.
  
  // R0 has the address of PortA w/Data Bits (0x400043F0 :: 0b0100.0011.1111.0000)
  // R4 has C1 - A (0x1F90  :: 0b0001.1111.1001.0000)
  // R5 has E  - A (0x1FD0C :: 0b0001.1111.1101.0000.1100) >> 2 (0b0000.0111.1111.0100.0011)
  
  // STR R5 into (R0 + R4)         //PortC //requires 010***** register 
  // STR R2 into (R0 + 3340)       //PortB
  // STR R1 into (R0 + (R5 << 2))  //PortE
  // STR R3 into (R0)              //PortA
  // STR R0 into (R0 + R4)         //PortC //requires 111***** register
  
	// First set CS and WR low, RD to high 111*****
	// Using DATA_BIT_R for PORTC w/mask for upper 3 bits
  // 0x40006000 + (0b11100000 << 2) -> (0x0D0 << 2 -> 0x380) = 0x40006380 << Better, probably
  // Alt: write to pins 5 + 7 w/ 0x0A0 -> 0x280 + 0x40006000 = 0x40006280 << Easier?

  // C1 to A: Diff: 0x1F90 -> 0b0001.1111.1001.0000 -> 8080
  // C2 to A: Diff: 0x1EF0 -> 0b0001.1110.1001.0000 -> 7824

  // DATA_BIT_R for PORTA for pins 2 to 7:
	// 0x40004000 + (0b11111100 << 2) -> (0x0FC << 2 -> 0x3F0) = 0x400043F0
  
  // C1 to B: Diff: 0x1284 -> 0b0001.0010.1000.0100 -> 4740
  // C2 to B: Diff: 0x1184 -> 0b0001.0001.1000.0100 -> 4484 
  // B  to A: Diff: 0x0D0C -> 0b0000.1101.0000.1100 -> 3340
  
  // DATA_BIT_R for PORTB for pins 0 to 5:
  // 0x40005000 + (0b00111111 << 2) -> (0x03F << 2 -> 0x0FC) = 0x400050FC
  
  // E to C1: Diff: 0x1DD7C -> 0b0001.1101.1101.0111.1100 -> 122236
  // E to C2: Diff: 0x1DE7C -> 0b0001.1101.1110.0111.1100 -> 122492
  // E to  B: Diff: 0x1F000 -> 0b0001.1111.0000.0000.0000 -> 126976 !!!
  // E to  A: Diff: 0x1FD0C -> 0b0001.1111.1101.0000.1100 -> 130316
  
  // DATA_BIT_R for PORTE for pins 0 to 5:
  // 0x40024000 + (0b00111111 << 2) -> (0x03F << 2 -> 0x0FC) = 0x400240FC
  
  // LDR/STR immediate offset: -255 to 4095 w/o pre/post
  //            w/pre/post is: -255 to 255

  // LDR/STR  register offset: Register and LSL of 0 to 3
  
//	LDR R0, =GPIO_PORTC_DATA_R
//	STR R1, R0
//	
//	GPIO_PORTF_DATA_R = 0xFF;
}



static void inline setRGB(register uint8_t r, register uint8_t g, register uint8_t b)
{
  register uint32_t aAddr        = (uint32_t)(GPIO_PORTA_DATA_BITS_R + (0x0FC << 2));
  register uint32_t a2cAddr      = (uint32_t)((GPIO_PORTC_DATA_BITS_R + (0x0D0 << 2)) - (GPIO_PORTA_DATA_BITS_R + (0x0FC << 2)));
  register uint32_t a2eShiftAddr = (uint32_t)(((GPIO_PORTE_DATA_BITS_R + (0x03F << 2)) - (GPIO_PORTA_DATA_BITS_R + (0x0FC << 2))) >> 2);

  // STR R5 into (R0 + R4)         //PortC //requires 010***** register 
  // STR R2 into (R0 + 3340)       //PortB
  // STR R1 into (R0 + (R5 << 2))  //PortE
  // STR R3 into (R0)              //PortA
  // STR R0 into (R0 + R4)         //PortC //requires 111***** register
  
  __asm ("ADD R0, %[input_i], %[input_j]"
    :  /* This is an empty output operand list */
    : [input_i] "r" (i), [input_j] "r" (j)
  );
  
  __asm __volatile__ (
        "lidt %0 \n" : : "m"(ptr) );
  
  __asm volatile
  (
    "STR %[A2E_Offset], [%[PortA_Addr]], %[A2C_Offset]] \n"
    "STR %[G], [, [%[PortA_Addr]] {, #3340}] \n"
    "STR %[R], [%[PortA_Addr]], %[A2E_Offset] {, LSL #2}] \n"
    "STR %[B], [%[PortA_Addr]] \n"
    "STR %[PortA_Addr]], [%[PortA_Addr]], R4] \n"
  :
  : [PortA_Addr] "r" (aAddr), [A2C_Offset] "r" (a2cAddr), [A2E_Offset] "r" (a2eShiftAddr), [R] "r" (r), [G] "r" (g), [B] "r" (b)
  //: "memory"
  );

}

void test(void)
{
   register uint8_t r = 0x3F;
   register uint8_t g = 0x3F;
   register uint8_t b = 0x1F;
  
  for(int i = 0; i < 76800; i++)
  {
    setRGB(r, g, b);
  }
}

int main(void)
{
	initIO();
	
	testUnfriendly();
  
  test();
}
