// graphic.h
// Runs on LM4F120/TM4C123
// graphic and collision calculation for PotatoDoom
// Combined graphic and physics module because a lot of shared calculation
// Steven Zhu

#define W 160 
#define H 96		// 128 - 32 = 96

#define hfov	90	// Horizontal field of view
#define vfov	60	// Vertical field of view
#define focalX	80	// =(W/2)/(tan(hfov/2))
#define focalY	83	// =(H/2)/(tan(vfov/2))	for seperate focal lengths
#define tanFOV	1	// tan(fov/2)	used for clipping, only horizontal fov matters
#define xFOV	1		// x component of FOV, used to speed up processing
#define yFOV	1		// y component of FOV, used to speed up processing
#define maxSeg	128

// Defines for texture list for flexibility
#define wall 0
#define door 1
#define browngnd 2

// Floor and ceiling color
#define floor 0xC618	// lightgray
#define ceiling 0x7BEF		// darkgray

#define nodeCount 235 // sizeof(nodeList) / 28 - 1

// Collision defines
#define detectionMultiplier 5	// Increase collision detection range
#define maxCollision	32	// number of segment to check for collision

#ifndef enemyCount
#define enemyCount 18	// Number of enemy
#endif

extern Player player;
extern uint8_t shooting;
extern float pcos, psin;
extern Enemy enemyList[];

//*****displayEnd*****
// Initialize LCD, HUD and gun
void graphicInit(void);

//*****drawScreen*****
// Resets counter and buffer and calls the various component 
// of graphics in order to draw the screen
void drawScreen(void);

//*****displayTitle*****
// Display the title screen until player start
void displayTitle(void);

//*****displayEnd*****
// Displays the ending screen
void displayEnd(void);
