// PWM Layer for Audio Output Purposes
// Written for the TM4C/LM4F, entirely untested
// Using PE5 with PWM Module 0 Output 5, Generator 2B'
//
// Rahul Butani
// April 30th, 2017

#include "pwm.h"

#include "../../contrib/tm4c123gh6pm.h"

void pwmHardwareInit()
{
  // Enable PWM Clock
  SYSCTL_RCGC0_R |= SYSCTL_RCGC0_PWM0;
  
  // Enable PortE Clock
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
  
  // PortE Initialization (PWM Output Pin for audio is PE5, as mentioned):
  // PE5 (5th bit -> 0x20)
  GPIO_PORTE_LOCK_R   =  0x4C4F434B;		      // Unlock GPIO_CR (Commit) Register
	GPIO_PORTE_CR_R    |=  0x20;					      // Allow changes to selected pin's configuration to persist
	GPIO_PORTE_DIR_R   |=  0x20;				        // Set pins as outputs
	GPIO_PORTE_DEN_R   |=  0x20;      					// Enable digital on pins
	GPIO_PORTE_AMSEL_R &= ~0x20;      					// Disable analog mode	
  GPIO_PORTE_AFSEL_R |=  0x20;                // Enable Alternate Function
  GPIO_PORTE_PCTL_R  |=  GPIO_PCTL_PE5_M0PWM5;// Enable PWM Module 0 on PE5
  
  // Clear USEPWMDIV bit in RCC to ensure that the PWM Clock = System Clock
  SYSCTL_RCC_R &= ~SYSCTL_RCC_USEPWMDIV;
  
  // Configure PWM0_2
  PWM0_2_CTL_R  |=  PWM_2_CTL_DBCTLUPD_I;    // Set to Immediate Update Mode
  // Set GeneratorA to do nothing (all bits 0)
  PWM0_2_GENA_R &=  PWM_2_GENA_ACTCMPBD_NONE;
  // Set GeneratorB to zero on compareB and 1 on load
  PWM0_2_GENB_R |= (PWM_2_GENB_ACTCMPBD_ZERO | PWM_2_GENB_ACTLOAD_ONE);
  
  // Set PWM Params:
  //
  // We're assuming the source clock is 80 MHz and that we want proper 44.1
  // KHz (to be able to output audio for the full range of human hearing - 
  // ~20 Hz to ~22KHz; Nyquist's Theoram doubles and puts us at 44.1 KHz).
  // In other words, in order to be able to output sound for the full range
  // of human hearing, we need to be able to output new analog values at at
  // least 44.1 KHz.
  //
  // This means that we expect to get a new 'analog' output value at a rate
  // of 44.1 KHz; since our PWM Clock is 80 MHz this means that we have 
  // 1814.05896 of our 12.5ns 80 MHz cycles to represent each value.
  //
  // Here's where the trade-off between really bad audio happens. If we want
  // as much precision as possible, we would choose to have 1 PWM cycle per
  // analog value; that way we would be able to represent 1815 distinct values
  // with our system, putting us somewhere between a 10/11 bit DAC. The issue
  // with this is that our output doesn't really look like an analog signal
  // anymore. The PWM magic only works if you have fast cycles... or a slow
  // sampling rate. However in this example, each PWM pulse is 2.2675737e-5 
  // seconds which might (?) work..
  //
  // If it doesn't the solution is to increase the number of PWM cycles per 
  // analog value... which decreases the precision.
  //
  // Another thing worth mentioning is that running at 44.1KHz means the
  // processor has to update the output value very often (every ~1814 cycles).
  // This puts significant strain on the system, especially if calculation or
  // data manipulation is required to get the next audio value.
  //
  // To make the numbers a little nicer, let's test with 39.0625 KHz audio.
  // 80 MHz / 2048 = 39.0625 KHz; a mutilated 12 bit DAC
  //
  // This means our PWM period should be 2048
  PWM0_2_LOAD_R = (2048 - 1);

  // Next, our duty cycle. This will vary and be set according to the current
  // audio value. This will be a 12 bit value (note: PWM hardware has a 16 bit
  // limit on this board) from 0 to 2047.
  // CMPB is set to this value so that the signal goes to 3.3V at this point
  // in the count down (this is why inversion is enabled).
  // Let's start off with a nice 50% duty cycle
  PWM0_2_CMPB_R = 1024;
  
  // Now let's enable the timers in this PWM Generator (2)
  PWM0_2_CTL_R |= PWM_2_CTL_ENABLE;
  
  // Invert output on PE5:
  PWM0_INVERT_R |= PWM_INVERT_PWM5INV;
  
  // And finally, let's enable the PWM Output on E5:
  // (We're using PWM Module 0, Generator 2B', Output 5 with PE5)
  PWM0_ENABLE_R |= PWM_ENABLE_PWM5EN;
}

void audioValueOut(uint16_t out)
{
  // This should be a value between 0 and 2047, but no safety in the interest of performance.
  PWM0_2_CMPB_R = out;
}
