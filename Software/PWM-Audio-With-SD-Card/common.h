#ifndef _COMMON_JANKPAD_
#define _COMMON_JANKPAD_

#include <stdint.h>
#include "contrib/tm4c123gh6pm.h"

#define debugOutputLCD  0
#define debugOutputUART 1

#define audioBufferSize 2048

void EnableInterrupts(void);

typedef enum { false, true } boolean;

typedef struct
{
	boolean (*init)(void);
	void (*setWindow)(uint16_t sX, uint16_t sY, uint16_t eX, uint16_t eY);
	void (*setCoordinate)(uint16_t X, uint16_t Y);
	void (*setPixel)(uint8_t r, uint8_t g, uint8_t b);
} LCD_Driver;

#endif //_COMMON_JANKPAD_
