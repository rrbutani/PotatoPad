// graphic.h
// Runs on LM4F120/TM4C123
// graphic and collision calculation for PotatoDoom
// Steven Zhu

#define W 160 
#define H 128		// 240 - 24 = 216

#define hfov	90	// Horizontal field of view
#define vfov	60	// Vertical field of view
#define focalX	80	// =(W/2)/(tan(hfov/2))
#define focalY	110	// =(H/2)/(tan(vfov/2))	for seperate focal lengths
#define tanFOV	1	// tan(fov/2)	used for clipping, only horizontal fov matters
#define xFOV	1		// x component of FOV, used to speed up processing
#define yFOV	1		// y component of FOV, used to speed up processing
#define maxSeg	128
#define maxCollision	8	// number of segment to check for collision
#define detectionMultiplier 5	// Increase collision detection range

// Defines for texture list for flexibility
#define wall 0
#define door 1
#define browngnd 2

#define nodeNum 235 // sizeof(nodeList) / 28 - 1

extern Player player;

// Initialize variables
void graphicInit(void);

// Resets buffer and render scene
void drawScreen(void);

void drawEnemy(void);

// Turns player
void turnPlayer(float angularSpeed);

// Check for collision, move player only if successful
void movePlayer(float speed);
