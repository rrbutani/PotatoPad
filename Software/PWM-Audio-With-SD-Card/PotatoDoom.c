// PotatoDoom.c
// Runs on LM4F120/TM4C123
// Main program of Potato Doom
// Contains main loop which calls different modules
// Steven Zhu

#include <stdint.h>
#include <math.h>

#include "contrib/tm4c123gh6pm.h"
#include "contrib/PLL.h"
#include "contrib/FPU.h"
#include "types.h"
#include "graphic.h"

void EnableInterrupts(void);

#define PI	3.1415927f	//3.14159265358979323846
#define enemyCount 18	// Number of enemy

// Global variables
Player player = {1056,-3520,45,PI/2,100};	// 1056, -3520	start, 1032, -3232 court
Enemy enemyList[] = {{2200,-3112,20},{2240,-3480,20},{1832,-3216,20},{2040,-2328,20},{2040,-2635,20},{2264,-4016,20},{2736,-2680,20},{2992,-3088,20},{3272,-3360,20},{3688,-3216,20},{3288,-3816,20},{2912,-4448,20},{3064,-4448,20},{144,-3144,20},{144,-3328,20},{-424,-3520,20},{-424,-2952,20},{-624,-3200,20}};
uint8_t shooting;	// Flag for renderer to draw gun fire
float pcos, psin;

// Private variables
static uint8_t ready;
	
// temp debug variables
float speed;
float angularSpeed;
	
static void playerInit(void) {
	player.x = 1056;
	player.y = -3520;
	player.z = 45;
	player.angle = PI/2;
	player.health = 100;
	player.armor = 100;
	player.speed = 0;
	player.score = 0;
	shooting = 0;
}

static void PortF_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x20;	// Heartbeat
	while ((SYSCTL_PRGPIO_R & 0x20) == 0) {}
	GPIO_PORTF_DIR_R = 0x0E;
	GPIO_PORTF_PUR_R |= 0x10;
	GPIO_PORTF_DEN_R |= 0x1E;
}

// SysTick used to maintain framerate
static void SysTick_Init(void) {
	NVIC_ST_CTRL_R = 0x00;
	NVIC_ST_RELOAD_R = 0x0028B0AA;	// 30 FPS
	NVIC_ST_CURRENT_R = 0x00;
	NVIC_ST_CTRL_R = 0x07;
}

//void SysTick_Handler(void) {
//	ready = 1;
//}

int main2(void){
	PLL_Init(); 		// Set clock to 80MHz	
	FPULazyStackingEnable();
	SysTick_Init();
	PortF_Init();
	EnableInterrupts();
	
	playerInit();
	graphicInit();
	
	// temp debug variables
	speed = 3;
	angularSpeed = -0.02;
//	player.x = 1740;
//	player.y = -3240;
//	player.angle = 0;
//	pcos = cos(player.angle);
//	psin = sin(player.angle);
	uint16_t counter = 0;

	displayTitle();
	ready = 0;
		
	while(1) {
		if (ready) {
			GPIO_PORTF_DATA_R ^= 0x02;	// Heartbeat
			ready = 0;
			drawScreen();
			// Update player position using joystick input
			counter = (counter+1) % 50;
			if (counter < 25) shooting = 1;
			else	shooting = 0;
			player.angle += angularSpeed;
			pcos = cos(player.angle);
			psin = sin(player.angle);
			player.speed = speed;
		}
	}
}
