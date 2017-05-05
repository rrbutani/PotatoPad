// audio.c
// Used to output background music and sound effect
// Uses PWM for output and SD for input
// Steven Zhu and Rahul Butani
// May 2, 2017

#include <stdio.h>
#include "contrib/tm4c123gh6pm.h"

#include "drivers/pwm.h"
#include "drivers/sd_SPI/diskio.h"
#include "drivers/sd_SPI/ff.h"
#include "drivers/ST7735/ST7735.h"
#include "common.h"
#include "Fifo.h"
#include "audio.h"
#include "SfxData.h"

//#define SD_CLOSE() SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+4; \
//	SSI0_CC_R = (SSI0_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_SYSPLL;
//#define SD_OPEN() 	SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+2; \
//  SSI0_CC_R = (SSI0_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_PIOSC;
#define SD_CLOSE()	SSI0_CC_R = (SSI0_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_SYSPLL
#define SD_OPEN() 	SSI0_CC_R = (SSI0_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_PIOSC

// Sound Effects we currently have
sfx playerHurt  = {playerHurtSFX, sizeof(playerHurtSFX)};
sfx enemyHurt   = {enemyHurtSFX, sizeof(enemyHurtSFX)};
sfx pistol      = {pistolSFX, sizeof(pistolSFX)};
sfx enemyDeath  = {enemyDeathSFX, sizeof(enemyDeathSFX)};
sfx playerDeath = {playerDeathSFX, sizeof(playerDeathSFX)};

// Array of Channels
static Channel audioChannels[maxAudioChannels];

static boolean SD_MOUNTED = false;

static FATFS g_sFatFs;
static FIL SongHandle;
static FRESULT Fresult;
static UINT numBytesRead = 0;

static uint32_t songSize = 0, songPlayerIdx;
static uint16_t songBuffer[songBufferSize];

static uint8_t numChannelsPlaying;

static void populateSongBuffer(void);

static void initSongBuffer(TCHAR *songName)
{
	if(SD_MOUNTED)
    SD_OPEN();
  else
    SD_MOUNTED = true;
  
	Fresult = f_open(&SongHandle, songName, FA_READ);
  if(Fresult == FR_OK)
  {
    Fresult = f_read(&SongHandle, &songSize, 4, &numBytesRead);

    if(numBytesRead == 4 && Fresult == FR_OK)
      Fresult = f_read(&SongHandle, &songBuffer, 2 * songBufferSize, &numBytesRead);
  }
  else
  {
    // printf("reading card didn't work...");
    initSongBuffer("FUN.AUD");
  }
  
  populateSongBuffer();
  
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
    f_lseek(&SongHandle, 4); // Move back to start of file
    Fresult = f_read(&SongHandle, &songBuffer, 2 * songBufferSize, &numBytesRead);
  }
  else
    Fresult = f_read(&SongHandle, &songBuffer, 2 * songBufferSize, &numBytesRead);
	
  songPlayerIdx = 0;
  
	SD_CLOSE();
}

//static void initSfxBuffer(TCHAR *sfxName)
//{
//	SD_OPEN();
//	
//	Fresult = f_open(&SfxHandle, sfxName, FA_READ);
//	if(Fresult == FR_OK)
//	{
//		Fresult = f_read(&SfxHandle, &sfxSize, 4, &numBytesRead);
//		if(numBytesRead == 4 && Fresult == FR_OK)
//		{
//			Fresult = f_read(&SfxHandle, &sfxBuffer, 2 * sfxBufferSize, &numBytesRead);
//		}
//	}
//	
//	SD_CLOSE();
//}



//// Returns false once the sound effect is finished.
//static boolean populateSfxBuffer(void)
//{
//	static uint32_t valueCount = 0;
//	SD_OPEN();
//	
//	valueCount += sfxBufferSize;
//	
//	if(valueCount > sfxSize)
//	{
//		valueCount = 0;
//		f_close(&SfxHandle);
//		return false;
//	}
//	
//	Fresult = f_read(&SfxHandle, &sfxBuffer, 2 * sfxBufferSize, &numBytesRead);
//	SD_CLOSE();
//	
//	return true;
//}


void initAudioChannels()
{
  for(uint8_t i = 0; i < maxAudioChannels; i++)
  {
    audioChannels[i] = (Channel){false, pistol, 0};
  }
  
  numChannelsPlaying = 0;
}

void audioInit(void) {
	pwmHardwareInit();
	
//  if(f_mount(&g_sFatFs, "", 0))
//    printf("SD Card Mount Error.");
	
	f_mount(&g_sFatFs, "", 0);
	
	FiFo_Init();
	initSongBuffer("d_e1m1.AUD");
	
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

// *****playSFX*****
// Plays sound effect, returns if we have too many sound effects playing
// Input: a sound effect struct
void playSFX(sfx soundEffect)
{
  // If we're out of empty channels, return
  if(numChannelsPlaying == maxAudioChannels)
    return;
  
  uint8_t i = 0;
  
  // Find the next empty audio channel
  while(audioChannels[i].playing == true)
    i++;
  
  audioChannels[i] = (Channel){true, soundEffect, 0};
  numChannelsPlaying++;
  
  return;
}

// *****playSong******
// Plays a song; replaces the current song.
// Input: the name of a song that corresponds to a file on the SD Card
void playSong(char *songName)
{
  initSongBuffer(songName);
}

// *****startMusic*****
// Toggle background music
void toggleMusic(void) {
	TIMER0_CTL_R ^= 0x01;    // Enable Timer0A
}

// To be called @ 7.805 KHz
uint16_t sfxCollectiveAudioOut(void)
{
  uint32_t sum = 0;
  
  for(uint8_t i = 0; i < maxAudioChannels; i++)
  {
      if(audioChannels[i].playing)
      {
        if(++audioChannels[i].counter == audioChannels[i].soundEffect.audioArraySize)
        {
          audioChannels[i].playing = false;
          numChannelsPlaying--;
        }
        sum += audioChannels[i].soundEffect.audioValuesArray[audioChannels[i].counter];
      }
  }
  
  // Scale up to 1024; 1/2 of our maximum output value/volume
  return (uint16_t)(2 * sum);
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT; // Acknowledge Timer0A
	static uint8_t sfxCounter = 0;
	static uint16_t sfxValue = 0;
	
	if(!sfxCounter--)
  {  
    sfxValue = sfxCollectiveAudioOut();
    sfxCounter = 4;
  }
  
	if(++songPlayerIdx == songBufferSize)
	{
		populateSongBuffer();
	}
		
	audioValueOut((songBuffer[songPlayerIdx])/2 + (sfxValue));
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
