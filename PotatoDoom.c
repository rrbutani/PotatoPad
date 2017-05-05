// PotatoDoom.c
// Runs on LM4F120/TM4C123
// Main program of Potato Doom
// Contains main loop which calls different modules
// Steven Zhu

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "contrib/tm4c123gh6pm.h"
#include "contrib/PLL.h"
#include "contrib/FPU.h"
#include "common.h"
#include "graphic.h"
#include "physic.h"
#include "input.h"
#include "audio.h"

void DisableInterrupts(void);
void EnableInterrupts(void);

// Global variables
Player player;	// 1056, -3520	start, 1032, -3232 court
Enemy enemyList[] = {{2200,-3112,20,0},{2240,-3480,20,0},{1832,-3216,20,0},{2040,-2328,20,0},{2040,-2635,20,0},{2264,-4016,20,0},{2736,-2680,20,0},{2992,-3088,20,0},{3272,-3360,20,0},{3688,-3216,20,0},{3288,-3816,20,0},{2912,-4448,20,0},{3064,-4448,20,0},{144,-3144,20,0},{144,-3328,20,0},{-424,-3520,20,0},{-424,-2952,20,0},{-624,-3200,20,0}};
float pcos, psin;

// Private variables
static boolean ready;
	
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
	player.running = 1;
	player.shooting = false;
}

// SysTick used to maintain framerate
static void SysTickInit(void) {
	NVIC_ST_CTRL_R = 0x00;
	NVIC_ST_RELOAD_R = 0x0028B0AA;	// 30 FPS
	NVIC_ST_CURRENT_R = 0x00;
	NVIC_ST_CTRL_R = 0x07;
}

void SysTick_Handler(void) {
	ready = true;
}

int main(void){
	PLL_Init(); 		// Set clock to 80MHz	
	FPULazyStackingEnable();
	
	playerInit();
	audioInit();
//	DisableInterrupts();
	graphicInit();
	inputInit();
	EnableInterrupts();
	
	// temp debug variables
//	player.speed = 1;
//	angularSpeed = -0.02;
//	player.x = 1740;
//	player.y = -3240;
//	player.angle = 0;
//	pcos = cos(player.angle);
//	psin = sin(player.angle);
//	uint16_t counter = 0;

	displayTitle();
	
	SysTickInit();
	enableInput();
	ready = false;

	while(1) {
		if (ready) {
			GPIO_PORTF_DATA_R ^= 0x02;	// Heartbeat
			ready = false;
			player.angle += player.angularSpeed;
			pcos = cos(player.angle);
			psin = sin(player.angle);
			movePlayer();
			drawScreen();
			// Completion requirement
			if (player.y < -4592) {
				displayEnd(1);
				// break;	possibly use botton to restart
			}
			if (player.health % 20 == 0)
				playSound(playerHurt);
			if (player.health < 0) {
				playSound(playerDeath);
				player.health = 0;
//        displayEnd(0);
				// break;	possibly use botton to restart
			}
		}
		// Update player speed using joystick input
		updateSpeed();
	}
}
