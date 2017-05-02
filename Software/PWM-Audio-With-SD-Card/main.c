#include <stdio.h>

#include "contrib/UART.h"
#include "contrib/PLL.h"
#include "contrib/FPU.h"

#include "common.h"

#include "drivers/pwm/pwm.h"
#include "drivers/sd_SPI/diskio.h"
#include "drivers/sd_SPI/ff.h"
#include "drivers/ST7735/ST7735.h"

static FATFS g_sFatFs;
FIL Handle;
FRESULT Fresult;
UINT numBytesRead = 0;

uint32_t audioSize = 0;
uint32_t currentAudioLoc = 0;
uint16_t audioBuffer[audioBufferSize];

boolean setup(void)
{
  PLL_Init();     // Set clock speed
	Output_Init();  // Setup USB UART
	
  FPULazyStackingEnable();
  
	//ST7735_InitR(INITR_REDTAB);
  //ST7735_FillScreen(0);
  
  pwmHardwareInit();
  
  if(f_mount(&g_sFatFs, "", 0))
    printf("SD Card Mount Error.");
  
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
	
	// Initialize PortF (F0 - F4; 0b00011111 -> 0x1F)
	GPIO_PORTF_LOCK_R   =  0x4C4F434B;		// Unlock GPIO_CR (Commit) Register
	GPIO_PORTF_CR_R    |=  0x1F;					// Allow changes to selected pin's configuration to persist
	GPIO_PORTF_DIR_R   |=  0x1F;				  // Set pins as outputs
	GPIO_PORTF_DEN_R   |=  0x1F;					// Enable digital on pins
	GPIO_PORTF_AMSEL_R &= ~0x1F;					// Disable analog mode
	GPIO_PORTF_AFSEL_R &= ~0x1F;					// Disable alternate mode
  
  EnableInterrupts();
  
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

void initAudioBuffer()
{
  printf("Audio Buffer Size: %d\n", audioBufferSize);
  
  Fresult = f_open(&Handle, "QUEEN.AUD", FA_READ);
  if(Fresult == FR_OK)
  {
    Fresult = f_read(&Handle, &audioSize, 4, &numBytesRead);
    printf("Read audio file; size is %lu\n",(long unsigned)audioSize);
    if(numBytesRead == 4 && Fresult == FR_OK)
    {
      Fresult = f_read(&Handle, &audioBuffer, 2 * audioBufferSize, &numBytesRead);
    }
  }
  else
  {
    printf("reading card didn't work...");
  }
}

int main(void)
{ 
  setup();
	printf("hello\n");

  initAudioBuffer();
  
  NVIC_ST_CTRL_R = 0x00;
	NVIC_ST_RELOAD_R = 2048;	//39.0625 KHz
	NVIC_ST_CURRENT_R = 0x00;
	NVIC_ST_CTRL_R = 0x07;
  
  while(1);
}

void populateBuffer(void)
{
  static uint32_t valueCount = 0;
  
  //blink();
  blink();
  //printf("Repopulating buffer..");
  
  valueCount += audioBufferSize;
  
  if(valueCount > audioSize)
  {
    valueCount = 0;
    f_close(&Handle);
    initAudioBuffer();
  }
  else
    Fresult = f_read(&Handle, &audioBuffer, 2 * audioBufferSize, &numBytesRead);

  blink();
}

void SysTick_Handler(void)
{
  static uint32_t i = 0;
  
  //printf(".");
  
  if(++i == audioBufferSize)
  {
    populateBuffer();
    i = 0;
  }
  
  audioValueOut((audioBuffer[i]));
}


