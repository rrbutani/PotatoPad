// audio.c
// Used to output background music and sound effect
// Uses PWM for output and SD for input
// Steven Zhu and Rahul Butani
// May 2, 2017

#include <stdio.h>
#include "tm4c123gh6pm.h"

//#include "drivers/pwm/pwm.h"
//#include "drivers/sd_SPI/diskio.h"
//#include "drivers/sd_SPI/ff.h"
//#include "drivers/ST7735/ST7735.h"

#include "ST7735.h"

#include "pwm.h"
#include "diskio.h"
#include "ff.h"
#include "Fifo.h"
#include "audio.h"

typedef enum { false, true } boolean;

static FATFS g_sFatFs;
static FIL Handle;
static FRESULT Fresult;
static UINT numBytesRead = 0;

static uint32_t audioSize = 0;
static uint32_t currentAudioLoc = 0;
static uint16_t audioBuffer[audioBufferSize];

static uint8_t playing;

static void initAudioBuffer()
{
//  printf("Audio Buffer Size: %d\n", audioBufferSize);
  
//  SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+10; // must be even number; originally 10
	Fresult = f_open(&Handle, "d_e1m1.AUD", FA_READ);
  if(Fresult == FR_OK)
  {
    Fresult = f_read(&Handle, &audioSize, 4, &numBytesRead);
//    printf("Read audio file; size is %lu\n",(long unsigned)audioSize);
    if(numBytesRead == 4 && Fresult == FR_OK)
    {
      Fresult = f_read(&Handle, &audioBuffer, 2 * audioBufferSize, &numBytesRead);
    }
  }
  else
  {
//    printf("reading card didn't work...");
  }
//	SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+2; // must be even number; originally 10
//	while(1){}
}

static void populateBuffer(void)
{	
	static uint32_t valueCount = 0;
  
  valueCount += audioBufferSize;
  
  if(valueCount > audioSize)
  {
    valueCount = 0;
//    SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+10; // must be even number; originally 10
		f_close(&Handle);
    initAudioBuffer();
//		SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+2; // must be even number; originally 10
  }
  else
    Fresult = f_read(&Handle, &audioBuffer, 2 * audioBufferSize, &numBytesRead);
}

void audioInit(void) {
	pwmHardwareInit();
  
  if(f_mount(&g_sFatFs, "", 0))
    printf("SD Card Mount Error.");
	
	initAudioBuffer();
	
	SYSCTL_RCGCTIMER_R |= 0x01;		// Only using Timer0
	while ((SYSCTL_RCGCTIMER_R & 0x01) == 0) {}
	//Timer0
  TIMER0_CTL_R = 0x00;    // Disable Timer
  TIMER0_CFG_R = 0x04;    // Put timer in 16-bit mode to split A and B
  TIMER0_TAMR_R = 0x02;   // Set mode to periodic for Timer 0A
	TIMER0_TBMR_R = 0x02;		// Set mode to periodic for Timer 0B
	TIMER0_TAILR_R = 2048;  // Load period for 0A 39.0625KHz
	TIMER0_TBILR_R = 0xFF;		// Load period for 0B
  TIMER0_TAPR_R = 0;      // Set clock0A multiplier
	TIMER0_TBPR_R = 0;   // Set clock0B multiplier
  TIMER0_ICR_R = 0x0101;    // Reset timeout flag
  TIMER0_IMR_R = 0x0101;    // Arm interrupt
		
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x80000000; // Set 0A to priority 4
	NVIC_PRI5_R = 0x00000080; // Set 0B to priority 4
		
	NVIC_EN0_R = 1 << 19 | 1 << 20;	// Enable interrupt for Timer0
	startMusic();
	//TIMER0_CTL_R = 0x0101;    // Enable Timer0B
		
	playing = 0;
}

//*****playSound*****
// Plays sound effect, return if already playing one
// Input: 0 for player hurt, 1 for enemy hurt, 2 for pistol, 3 for monster death
void playSound(uint8_t sound) {
	if (playing)	return;
	playing = 1;
	switch (sound) {
		case 0: 	// Monster
			break;
		case 1:
			break;
		case 2:
			break;
	}
}

// *****startMusic*****
// Start background music
void startMusic(void) {
	TIMER0_CTL_R |= 0x01;    // Enable Timer0A
}

// *****stopMusic*****
// Stop background music
void stopMusic(void) {
	TIMER0_CTL_R &= ~0x01;    // Disable Timer0A
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT; // Acknowledge Timer0A
	static uint32_t i = 0;
  
  //printf(".");
  
	if(++i == audioBufferSize)
	{
		populateBuffer();
		i = 0;
	}
		
	audioValueOut((audioBuffer[i]));
}

void Timer0B_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TBTOCINT; // Acknowledge Timer0B
}