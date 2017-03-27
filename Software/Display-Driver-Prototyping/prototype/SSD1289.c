/*
 * A driver for SSD1289 ICs in 16-bit 8080 parallel mode
 * written for the T4MC Launchpad.
 *
 * Pinout is hardcoded for now as follows:
 * DB00:05  -> PE0:5
 * DB06:10  -> PF0:4
 * DB11:15  -> PA2:6
 * D/C 	-> PC4
 * WR  	-> PC5
 * RD   -> PC6
 * CS   -> PC7
 * RST  -> PA7 // Not used for now, really connected to 3.3V power rail
 */
#include "SSD1289.h"
 
#include <stdint.h>
#include "common.h"
#include "tm4c123gh6pm.h"
 
 //#define dataMode()

typedef enum {OUTPUT, INPUT} io_mode_t;

typedef enum
{
	// Register addresses for the SSD1289 Controller
	// More information available in datasheet
	INDEX										= 0x00, // Register to write commands to (command mode)
	DEVICE_CODE							= 0x00, // Can be read from to get 0x8989
	OSCILLATION_CTRL        = 0x00, // The rest must be written to in
	DRIVER_OUTPUT_CTRL      = 0x01, // data mode.
	DRIVING_WAVEFORM_CTRL		= 0x02, 
	POWER_CTRL_1						= 0x03, // First write register address to
	COMPARE_1								= 0x05, // index in command mode, then 
	COMPARE_2								= 0x06, // write data for said register
	DISPLAY_CTRL						= 0x07, // in data mode.
	FRAME_CYCLE_CTRL				= 0x0B,
	POWER_CTRL_2						= 0x0C,
	POWER_CTRL_3						= 0x0D,
	POWER_CTRL_4						= 0x0E,
	GATE_SCAN_POSITION			= 0x0F,
	SLEEP_MODE							= 0x10,
	ENTRY_MODE							= 0x11,
	GENERIC_INTERFACE_CTRL  = 0x15,
	HORIZONTAL_PORCH				= 0x16,
	VERTICAL_PORCH					= 0x17,
	POWER_CTRL_5						= 0x1E,
	DATA_TO_GRAM						= 0x22, // Can be written to or read from
	RAM_WRITE_DATA_MASK_RG	= 0x23,
	RAM_WRITE_DATA_MASK_B		= 0x24,
	FRAME_FREQUENCY_CTRL		= 0x25,
	VCOM_OTP_1							= 0x28, // OEM Settings type stuff, shouldn't
	VCOM_OTP_2							= 0x29, // need to touch these
	GAMMA_CTRL_0						= 0x30,
	GAMMA_CTRL_1						= 0x31,
	GAMMA_CTRL_2						= 0x32,
	GAMMA_CTRL_3						= 0x33,
	GAMMA_CTRL_4						= 0x34,
	GAMMA_CTRL_5						= 0x35,
	GAMMA_CTRL_6						= 0x36,
	GAMMA_CTRL_7						= 0x37,
	GAMMA_CTRL_8						= 0x3A,
	GAMMA_CTRL_9						= 0x3B,
	VERTICAL_SCROLL_CTRL_1	= 0x41, // For 1st Screen
	VERTICAL_SCROLL_CTRL_2 	= 0x42, // For 2nd Screen (virtual)
	HORIZONTAL_RAM_ADDR_POS = 0x44,
	VERTICAL_RAM_ADDR_POS_S	= 0x45, // Vertical Window Start 
	VERTICAL_RAM_ADDR_POS_E = 0x46, // Vertical Window End
	FRST_SCREEN_DRVNG_POS_S	= 0x48,
	FRST_SCREEN_DRVNG_POS_E = 0x49,
	SCND_SCREEN_DRVNG_POS_S = 0x4A,
	SCND_SCREEN_DRVNG_POS_E = 0x4B,
	RAM_ADDRESS_SET_X				= 0x4E,
	RAM_ADDRESS_SET_Y				= 0x4F,
	OPTIMIZE_DATA_SPEED_1		= 0x28,
	OPTIMIZE_DATA_SPEED_2		= 0x2F,
	OPTIMIZE_DATA_SPEED_3		= 0x12
} ssd1289_register_t;

/* Configures and initializes the relevant IO pins for the LCD.
 * Returns nothing.
 */
static void initIO(void)
{
	// Start clock for Ports A, C, E, and F (0b00110101 -> 0x35)
	SYSCTL_RCGC2_R |= (SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOE | SYSCTL_RCGC2_GPIOF); 
	while ((SYSCTL_RCGC2_R & 0x35) != 0x35) {}		// Wait a few cycles
	
	// Initialize PortA (A2 - A6; 0b01111100 -> 0x7C)
	GPIO_PORTA_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTA_CR_R    |=  0x7C;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTA_DIR_R   |=  0x7C;				  // Set pins as outputs
	GPIO_PORTA_DEN_R   |=  0x7C;					// Enable digital on pins
	GPIO_PORTA_AMSEL_R &= ~0x7C;					// Disable analog mode
	GPIO_PORTA_AFSEL_R &= ~0x7C;					// Disable alternate mode
	GPIO_PORTA_PCTL_R  &= ~0x7C;					// Disable peripherals on port/clear port control

	// Initialize PortC (C4 - C7; 0b11110000 -> 0xF0)
	GPIO_PORTC_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTC_CR_R    |=  0xF0;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTC_DIR_R   |=  0xF0;				  // Set pins as outputs
	GPIO_PORTC_DEN_R   |=  0xF0;					// Enable digital on pins
	GPIO_PORTC_AMSEL_R &= ~0xF0;					// Disable analog mode
	GPIO_PORTC_AFSEL_R &= ~0xF0;					// Disable alternate mode
	GPIO_PORTC_PCTL_R  &= ~0xF0;					// Disable peripherals on port/clear port control
	
	// Initialize PortE (E0 - E5; 0b00111111 -> 0x3F)
	GPIO_PORTE_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTE_CR_R    |=  0x3F;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTE_DIR_R   |=  0x3F;				  // Set pins as outputs
	GPIO_PORTE_DEN_R   |=  0x3F;					// Enable digital on pins
	GPIO_PORTE_AMSEL_R &= ~0x3F;					// Disable analog mode
	GPIO_PORTE_AFSEL_R &= ~0x3F;					// Disable alternate mode
	GPIO_PORTE_PCTL_R  &= ~0x3F;					// Disable peripherals on port/clear port control

	// Initialize PortF (F0 - F4; 0b00011111 -> 0x1F)
	GPIO_PORTF_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTF_CR_R    |=  0x1F;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTF_DIR_R   |=  0x1F;				  // Set pins as outputs
	GPIO_PORTF_DEN_R   |=  0x1F;					// Enable digital on pins
	GPIO_PORTF_AMSEL_R &= ~0x1F;					// Disable analog mode
	GPIO_PORTF_AFSEL_R &= ~0x1F;					// Disable alternate mode
	GPIO_PORTF_PCTL_R  &= ~0x1F;					// Disable peripherals on port/clear port control

	// Clear bus pins:
	GPIO_PORTA_DATA_R  &= ~0x7C;
	GPIO_PORTE_DATA_R  &= ~0x3F;
	GPIO_PORTF_DATA_R  &= ~0x1F;
	
	// Set CS, D/C, WR, RD pins all to high:
	GPIO_PORTC_DATA_R  |=  0xF0;
}

/* Takes a 16 bit value and outputs it to the LCD Bus lines.
 * Returns nothing.
 * Note that this method does not set the D/C lines; that is to be done
 * prior to calling this method.
 */
static void writeBus(uint16_t halfword)
{
	// halfword: 0beeeeeefffffaaaaa
	// -> PortA: 0b*aaaaa**
	// -> PortE: 0b**eeeeee | There are no other pins on Port E and F
	// -> PortF: 0b***fffff | so we don't really _need_ to be friendly
	
	// First set the CS Line as well as the WR Line low 
	// (assuming RD is still high)
	// (note that CS doesn't _have_ to be set low, but it's good practice if we ever 
	// wish to reuse the bus and it doesn't cost anything in terms of speed)
	// (clear pin PC5 and PC7: 0b10100000 -> 0xA0)
	GPIO_PORTC_DATA_R &= ~0xA0;
	
	// Now write the data out to the bus:
	GPIO_PORTA_DATA_R  = (GPIO_PORTA_DATA_R & ~0x7C) | ((halfword << 2)  & 0x7C);
	GPIO_PORTF_DATA_R  = (GPIO_PORTF_DATA_R & ~0x1F) | ((halfword >> 5)  & 0x1F);
	GPIO_PORTE_DATA_R  = (GPIO_PORTE_DATA_R & ~0x3F) | ((halfword >> 10) & 0x3F); 
	
	// The above values will hold for far greater than 10 ns (as per tDSH and tDHW)
	// since 1 cycle is 12.5 ns here so we don't need to wait.
		
	// By now we'll have satisfied the PulseWidthLow write timing (50 ns) and tCYCLE
	// isn't really a concern (method runtime is >100 ns)
	// So we can set WR and CS back to high
	GPIO_PORTC_DATA_R |=  0xA0;
	
	// Calling this method immediately again (likely) isn't a concern.
  // (it's highly improbable that we'll take <100 ns to execute and break the Pulse
	// Width Low Time for writes and tCYCLE for writes)
}

/* Takes nothing.
 * Returns 16 bit value from the LCD Bus.
 * Note that this method does not set the D/C lines; that is to be done
 * prior to calling this method.
 * Not designed for consecutive reads!
 */
static uint16_t readBus(void)
{
	// PortA: 0b*aaaaa**
	// PortE: 0b**eeeeee
	// PortF: 0b***fffff
	// hword: 0beeeeeefffffaaaaa
	
	uint16_t halfword = 0x00;
	
	// First set the CS Line as well as the RD Line low 
	// (assuming WR is still high)
	// (note that CS doesn't _have_ to be set low, but it's good practice if we ever 
	// wish to reuse the bus and it doesn't cost anything in terms of speed)
	// (clear pin PC6 and PC7: 0b11000000 -> 0xC0)
	GPIO_PORTC_DATA_R &= ~0xC0;
	
	// Need to wait 250 ns (tACC) from setting RD low until valid data appears
	// 20 NOPs: kinda hacky but it works?
	__asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
	__asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
	__asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
	
	// Now actually read the data in:
	halfword |= ((GPIO_PORTE_DATA_R & 0x3F) << 10);
	halfword |= ((GPIO_PORTF_DATA_R & 0x1F) <<  5);
	halfword |= ((GPIO_PORTA_DATA_R & 0x7C) >>  2);
	
	// It's probably been 250 ns since we started reading data by now, so the 
	// pulse width low time for a read cycle (500 ns) should be satisfied by now.
	// Therefore, we can set CS and RD back to high:
	GPIO_PORTC_DATA_R  |=  0xC0;
	
	// From here, the RD line is supposedly required to stay high for 500 ns
	// to complete the cycle, but we won't busy wait for 500 ns because
	// consecutive reads are unlikely and the requirement seems questionable.
	
	return halfword;
}

/* Takes IO Mode (input or output) and sets LCD Bus accordingly.
 * Returns nothing.
 */
static void setBusMode(io_mode_t mode)
{
	switch(mode)
	{
		case OUTPUT:
				GPIO_PORTA_DIR_R |= 0x7C;
				GPIO_PORTE_DIR_R |= 0x3F;
				GPIO_PORTF_DIR_R |= 0x1F;
				break;
		
		case INPUT:
				GPIO_PORTA_DIR_R &= ~0x7C;
				GPIO_PORTE_DIR_R &= ~0x3F;
				GPIO_PORTF_DIR_R &= ~0x1F;
				break;
	}
}

/* Sets register in display controller.
 * Takes a register memory address and a desired 16 bit value.
 * Returns nothing.
 */
static void writeToRegister(ssd1289_register_t reg, uint16_t data)
{
	// Enter 'command' mode (essentially, set the index register; think MAR) by setting
	// D/C Line (PC4 -> 0b00010000 -> 0x10) low.
	GPIO_PORTC_DATA_R &= ~0x10;
	
	// Next write the register address out:
	writeBus(reg);
	
	// Now, switch back to 'data' mode to actually set the register.
	GPIO_PORTC_DATA_R |=  0x10;
	
	// And finally, write out the data we want to put in the register:
	writeBus(data);
}

/* Reads register from controller.
 * Takes a register memory address.
 * Returns value of given register.
 */
static uint16_t readFromRegister(ssd1289_register_t reg)
{
	uint16_t data;
	
	// Enter 'command' mode to specify which register we want to read from.
	// D/C Line (PC4 -> 0b00010000 -> 0x10) to low.
	GPIO_PORTC_DATA_R &= ~0x10;
	
	// Next write the register address out:
	writeBus(reg);
	
	// Now, switch back to 'data' mode to get data from the register.
	GPIO_PORTC_DATA_R |=  0x10;
	
	// Also set the data bus to input mode so we can actually read from it:
	setBusMode(INPUT);
	
	// Read the data from the register:
	data = readBus();
	
	// And finally reset the bus to output mode to play nice with others:
	setBusMode(OUTPUT);
	
	return data;
}

/* Tests if LCD is attached and functioning.
 * If so, configures/prepares LCD for use (sets registers and such).
 * Returns boolean indicating if LCD was found.
 */
static boolean setupLCD(void)
{
	// Read from device code register to see if device is present.
	readFromRegister(DEVICE_CODE);							// One 'dummy' read as per datasheet (p.21)
	if(readFromRegister(DEVICE_CODE) != 0x8989)	// Now the actual read; 0x8989 is expected value
		return false;
	
	// If device is present continue with device setup.
	writeToRegister(OSCILLATION_CTRL, 0x0001);
	writeToRegister(POWER_CTRL_1, 		0xA8A4);
	
	return true;
}

boolean initLCD(void)
{
	initIO();
	return setupLCD();
}
	
