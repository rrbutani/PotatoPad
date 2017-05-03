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

#define SD_CLOSE() SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+4; \
	SSI0_CC_R = (SSI0_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_SYSPLL;
#define SD_OPEN() 	SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+2; \
  SSI0_CC_R = (SSI0_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_PIOSC;

typedef enum { false, true } boolean;

static FATFS g_sFatFs;
static FIL SongHandle, SfxHandle;
static FRESULT Fresult;
static UINT numBytesRead = 0;

static uint32_t songSize = 0, sfxSize=0;
static uint16_t songBuffer[songBufferSize], sfxBuffer[sfxBufferSize];

static void initSongBuffer()
{
//  printf("Audio Buffer Size: %d\n", audioBufferSize);
	
	Fresult = f_open(&SongHandle, "d_e1m1.AUD", FA_READ);
  if(Fresult == FR_OK)
  {
    Fresult = f_read(&SongHandle, &songSize, 4, &numBytesRead);
//    printf("Read audio file; size is %lu\n",(long unsigned)audioSize);
    if(numBytesRead == 4 && Fresult == FR_OK)
    {
      Fresult = f_read(&SongHandle, &songBuffer, 2 * songBufferSize, &numBytesRead);
    }
  }
  else
  {
//    printf("reading card didn't work...");
  }
}

static void initSfxBuffer(TCHAR *sfxName)
{
	SD_OPEN();
	
	Fresult = f_open(&SfxHandle, sfxName, FA_READ);
	if(Fresult == FR_OK)
	{
		Fresult = f_read(&SfxHandle, &sfxSize, 4, &numBytesRead);
		if(numBytesRead == 4 && Fresult == FR_OK)
		{
			Fresult = f_read(&SfxHandle, &sfxBuffer, 2 * sfxBufferSize, &numBytesRead);
		}
	}
	
	SD_CLOSE();
}

static void populateSongBuffer(void)
{	
	static uint32_t valueCount = 0;
	SD_OPEN();
	
  valueCount += songBufferSize;
  
  if(valueCount > songSize)
  {
    valueCount = 0;
		f_close(&SongHandle);
    initSongBuffer();
	}
  else
    Fresult = f_read(&SongHandle, &songBuffer, 2 * songBufferSize, &numBytesRead);
	
	SD_CLOSE();
}

// Returns false once the sound effect is finished.
static boolean populateSfxBuffer(void)
{
	static uint32_t valueCount = 0;
	SD_OPEN();
	
	valueCount += sfxBufferSize;
	
	if(valueCount > sfxSize)
	{
		valueCount = 0;
		f_close(&SfxHandle);
		return false;
	}
	
	Fresult = f_read(&SfxHandle, &sfxBuffer, 2 * sfxBufferSize, &numBytesRead);
	SD_CLOSE();
	
	return true;
}

void audioInit(void) {
	pwmHardwareInit();
	
//  if(f_mount(&g_sFatFs, "", 0))
//    printf("SD Card Mount Error.");
	
	f_mount(&g_sFatFs, "", 0);
	
	FiFo_Init();
	initSongBuffer();
	
	SYSCTL_RCGCTIMER_R |= 0x01;		// Only using Timer0
	while ((SYSCTL_RCGCTIMER_R & 0x01) == 0) {}
	//Timer0
  TIMER0_CTL_R = 0x00;    // Disable Timer
  TIMER0_CFG_R = 0x04;    // Put timer in 16-bit mode to split A and B
  TIMER0_TAMR_R = 0x02;   // Set mode to periodic for Timer 0A
	TIMER0_TBMR_R = 0x02;		// Set mode to periodic for Timer 0B
	TIMER0_TAILR_R = 2048;  // Load period for 0A 39.0625KHz
	TIMER0_TBILR_R = 2048;		// Load period for 0B 39.0625KHz
  TIMER0_TAPR_R = 0;      // Set clock0A multiplier
	TIMER0_TBPR_R = 0;   // Set clock0B multiplier
  TIMER0_ICR_R = 0x0101;    // Reset timeout flag
  TIMER0_IMR_R = 0x0101;    // Arm interrupt
		
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x80000000; // Set 0A to priority 4
	NVIC_PRI5_R = 0x00000080; // Set 0B to priority 4
		
	NVIC_EN0_R = 1 << 19 | 1 << 20;	// Enable interrupt for Timer0
	TIMER0_CTL_R |= 0x01;    // Enable Timer0A
	//TIMER0_CTL_R = 0x0101;    // Enable Timer0B
}

//*****playSound*****
// Plays sound effect, return if already playing one
// Input: 0 for player hurt, 1 for enemy hurt, 2 for pistol, 3 for monster death
void playSound(uint8_t sound) {
	FiFo_Put(sound);
}

// *****startMusic*****
// Toggle background music
void toggleMusic(void) {
	TIMER0_CTL_R ^= 0x01;    // Enable Timer0A
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT; // Acknowledge Timer0A
	static uint32_t songIdx = 0;
	static uint32_t sfxIdx  = 0;
  static boolean sfxPlaying = false;
	static uint16_t sfxValue = 0;
	
	
	if(sfxPlaying)
	{
		if(++sfxIdx == sfxBufferSize)
		{
			sfxPlaying = populateSfxBuffer();
			sfxIdx = 0;
		}
		sfxValue = sfxBuffer[sfxIdx];
	}
	else
	{
		char newSfx;
		if(FiFo_Get(&newSfx))
		{
			sfxPlaying = true;
			switch(newSfx)
			{
				case playerHurt	: initSfxBuffer("pain.aud");
													break;
				case enemyHurt	: initSfxBuffer("ehurt.aud");
													break;
				case pistol			: initSfxBuffer("pistol.aud");
													break;
				case enemyDeath	: initSfxBuffer("edeath.aud");
													break;
				case playerDeath: initSfxBuffer("death.aud");
													break;
			}
		}
		sfxValue = 0;
	}
  
	if(++songIdx == songBufferSize)
	{
		populateSongBuffer();
		songIdx = 0;
	}
		
	audioValueOut((songBuffer[songIdx])/2 + (sfxValue)/2);
}

//void Timer0B_Handler(void){
//  TIMER0_ICR_R = TIMER_ICR_TBTOCINT; // Acknowledge Timer0B
//	static uint32_t i = 0;
//  
//  //printf(".");
//  
//	if(++i == songBufferSize)
//	{
//		populateBuffer();
//		i = 0;
//	}
//		
//	audioValueOut((audioBuffer[i]));
//}
