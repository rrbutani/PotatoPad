// graphic.h
// Runs on LM4F120/TM4C123
// graphic calculation for PotatoDoom
// Handle every graphic displayed including rendering scene and HUD
// Steven Zhu

#define W 160 
#define H 96		// 128 - 32 = 96 Height of display screen without HUD
#define screenHeight 128
#define HUDHeight 32

#define hfov	90	// Horizontal field of view
#define vfov	60	// Vertical field of view
#define focalX	80	// =(W/2)/(tan(hfov/2))
#define focalY	83	// =(H/2)/(tan(vfov/2))	for seperate focal lengths
#define tanFOV	1	// tan(fov/2)	used for clipping, only horizontal fov matters
#define xFOV	1		// x component of FOV, used to speed up processing
#define yFOV	1		// y component of FOV, used to speed up processing
#define maxSeg	128

// Defines for texture list for flexibility
#define startan3	0
#define door3	1
#define browngnd	2
#define lite3	3
#define	brown1	4
#define	stargr1	5
#define	comptile	6
#define	doortrak	7
#define support2	8
#define techwall	9

// Floor and ceiling color
#define floor 0xC618	// lightgray
#define ceiling 0x7BEF		// darkgray

#define nodeCount 235 // sizeof(nodeList) / 28 - 1

#define refreshRate	120	// Rate at which HUD refresh to correct any mistakes

extern Player player;
extern float pcos, psin;
extern Enemy enemyList[];
extern uint8_t collisionCount;
extern Seg collisionList[];	// List of segment to check against for collision


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
// Input: 0 for game over, 1 for victory
void displayEnd(uint8_t input);
