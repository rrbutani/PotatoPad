// PotatoDoom.c
// Runs on LM4F120/TM4C123
// Main program of Potato Doom
// Contains main loop which calls different modules
// Steven Zhu

#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "FPU.h"
#include "types.h"
#include "graphic.h"

void EnableInterrupts(void);

#define PI	3.1415927f	//3.14159265358979323846

Player player = {1056,-3520,45,PI/2,100};	// 1056, -3520	start, 1032, -3232 court

static void playerInit(void) {
	player.x = 1056;
	player.y = -3520;
	player.z = 45;
	player.angle = PI/2;
	player.health = 100;
}

int main(void){
	PLL_Init(); 		// Set clock to 80MHz	
	FPULazyStackingEnable();
	EnableInterrupts();
	playerInit();
	graphicInit();
	
	// temp debug variables
	float speed = 5;
	float angularSpeed = -0.02;
//	player.x = 1504;
//	player.y = -3200;
//	player.angle = PI/2;
	
	SYSCTL_RCGCGPIO_R |= 0x20;	// Heartbeat
	while ((SYSCTL_PRGPIO_R & 0x20) == 0) {}
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;

	while(1) {
		GPIO_PORTF_DATA_R ^= 0x02;
		drawScreen();
		drawEnemy();
		// Move player
		turnPlayer(angularSpeed);
		movePlayer(speed);
	}
}
