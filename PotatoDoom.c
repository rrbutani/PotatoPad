// PotatoDoom.c
// Runs on LM4F120/TM4C123
// Main program of Potato Doom
// Contains main loop which calls different modules
// Steven Zhu

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "FPU.h"
#include "types.h"
#include "graphic.h"
#include "physic.h"
#include "input.h"
#include "audio.h"

void EnableInterrupts(void);

#define PI	3.1415927f	//3.14159265358979323846
#define enemyCount 18	// Number of enemy

// Global variables
Player player;	// 1056, -3520	start, 1032, -3232 court
Enemy enemyList[] = {{2200,-3112,20,0},{2240,-3480,20,0},{1832,-3216,20,0},{2040,-2328,20,0},{2040,-2635,20,0},{2264,-4016,20,0},{2736,-2680,20,0},{2992,-3088,20,0},{3272,-3360,20,0},{3688,-3216,20,0},{3288,-3816,20,0},{2912,-4448,20,0},{3064,-4448,20,0},{144,-3144,20,0},{144,-3328,20,0},{-424,-3520,20,0},{-424,-2952,20,0},{-624,-3200,20,0}};
uint8_t shooting;	// Flag for player firing
float pcos, psin;

// Private variables
static uint8_t ready;
	
static void playerInit(void) {
	player.x = 1056;
	player.y = -3520;
	player.z = 45;
	player.speed = 0;
	player.angle = PI/2;
	player.angularSpeed = 0;
	player.health = 100;
	player.armor = 100;
	player.score = 0;
	shooting = 0;
}

// SysTick used to maintain framerate
static void SysTickInit(void) {
	NVIC_ST_CTRL_R = 0x00;
	NVIC_ST_RELOAD_R = 0x0028B0AA;	// 30 FPS
	NVIC_ST_CURRENT_R = 0x00;
	NVIC_ST_CTRL_R = 0x07;
}

void SysTick_Handler(void) {
	ready = 1;
}

int main(void){
	PLL_Init(); 		// Set clock to 80MHz	
	FPULazyStackingEnable();
	SysTickInit();
	playerInit();
	audioInit();
	graphicInit();
	inputInit();
	EnableInterrupts();
	
	// temp debug variables
//	speed = 3;
//	angularSpeed = -0.02;
//	player.x = 1740;
//	player.y = -3240;
//	player.angle = 0;
//	pcos = cos(player.angle);
//	psin = sin(player.angle);
//	uint16_t counter = 0;

	displayTitle();
	ready = 0;
	
//	player.score = 1000;
//	displayEnd(0);

	while(1) {
		if (ready) {
			GPIO_PORTF_DATA_R ^= 0x02;	// Heartbeat
			ready = 0;
			player.angle += player.angularSpeed;
			pcos = cos(player.angle);
			psin = sin(player.angle);
			movePlayer();
			// Completion requirement
			if (player.y < -4592) {
				stopMusic();
				displayEnd(1);
				// break;	possibly use botton to restart
			}		
			updateHealth();
			if (player.health < 0) {
				player.health = 100;
				//displayEnd(0);
				// break;	possibly use botton to restart
			}
			drawScreen();
		}
		// Update player speed using joystick input
		updateSpeed();
	}
}
