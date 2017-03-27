#include <stdio.h>

#include "UART.h"
#include "graphics.h"

void setup(void)
{
  UART_Init();
	//initalizeGraphics();
}

int main(void)
{
	setup();
	while(1){
		printf("hello\n");
	}
}
