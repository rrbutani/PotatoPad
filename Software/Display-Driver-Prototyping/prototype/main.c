#include "UART.h"
#include "SSD1289.h"

void setup(void)
{
  UART_Init();
	initLCD();
}

int main(void)
{
	setup();
}
