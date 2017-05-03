// physic.h
// Runs on LM4F120/TM4C123
// collision calculation for PotatoDoom
// also responsible for enemy line of sight
// Steven Zhu

// Collision defines
#ifndef COLLISION_DEF
#define maxCollision	32	// number of segment to check for collision
#endif
#define detectionMultiplier 5	// Increase collision detection range
#define collisionRange 10	// Increase collision detection range
#define rebound	2

// Line of sight defines
#define maxRange 1500

#ifndef enemyCount
#define enemyCount 18	// Number of enemy
#endif

#define firingCooldown	30	// Firing cooldown for enemies

extern Player player;
extern float pcos, psin;
extern const Vertex vertexList[];
extern Enemy enemyList[];
extern uint8_t enemyRenderCount;
extern uint8_t enemyRenderList[];

//*****movePlayer*****
// Checks every segment in collisionList against player move
// If collide, rebound at normal to collided segment
void movePlayer(void);

//*****updateHealth*****
// Update player health/armor base on enemy line of sight and cooldown status
void updateHealth(void);
