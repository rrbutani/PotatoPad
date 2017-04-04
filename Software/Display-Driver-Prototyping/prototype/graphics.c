#include "graphics.h"

#include "SSD1289.h"

#define LCD_DRIVER SSD1289

LCD_Driver display;

boolean initalizeGraphics(void)
{
	display = LCD_DRIVER;
	
	return display.init();
}
