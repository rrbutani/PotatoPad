// physic.c
// Runs on LM4F120/TM4C123
// collision calculation for PotatoDoom
// also responsible for enemy line of sight
// Steven Zhu

#include <stdint.h>
#include <math.h>
#include "common.h"
#include "physic.h"
#include "audio.h"

uint8_t collisionCount;
Seg collisionList[maxCollision];	// List of segment to check against for collision

//*****movePlayer*****
// Checks every segment in collisionList against player move
// If collide, rebound at normal to collided segment
void movePlayer(void) {
	uint8_t radius = player.speed > 0 ? collisionRange : -collisionRange;
	int8_t pdx = (player.speed*player.running + radius) * pcos;
	int8_t pdy = (player.speed*player.running + radius) * psin;
	for (uint8_t i = 0; i < collisionCount; i++) {
		Vertex v1 = vertexList[collisionList[i].startVertex];
		Vertex v2 = vertexList[collisionList[i].endVertex];
		int16_t dx = v2.x - v1.x;
		int16_t dy = v2.y - v1.y;
		
		// Check player movement		
		if (((crossProduct((player.x-v1.x),(player.y-v1.y),dx,dy) > 0) != (crossProduct((player.x+pdx-v1.x),(player.y+pdy-v1.y),dx,dy) > 0)) &&
			((crossProduct((v1.x-player.x),(v1.y-player.y),pdx,pdy) > 0) != (crossProduct((v2.x-player.x),(v2.y-player.y),pdx,pdy) > 0)))	{
			float hyp = sqrt(dx*dx+dy*dy);
			player.x -= dy / hyp * rebound;
			player.y -= dx / hyp * rebound;
			return;
		}
	}
	
	player.x += player.speed*player.running * pcos;
	player.y += player.speed*player.running * psin;
}

//*****updateHealth*****
// Update player health/armor base on enemy line of sight and cooldown status
void updateHealth(void) {
	// Check enemy line of sight
	enemyRenderCount = 0;
	for (uint8_t i = 0; i < enemyCount; i++) {
		if (enemyList[i].health <= 0)	continue;
		if (enemyList[i].cooldown != 0) enemyList[i].cooldown--;
		int16_t edx = enemyList[i].x - player.x;
		int16_t edy = enemyList[i].y - player.y;
		if (sqrt(edx*edx+edy*edy) > maxRange)	continue;
		
		uint8_t inView = 1;
		for (uint8_t i = 0; i < collisionCount; i++) {
			Vertex v1 = vertexList[collisionList[i].startVertex];
			Vertex v2 = vertexList[collisionList[i].endVertex];
			int16_t dx = v2.x - v1.x;
			int16_t dy = v2.y - v1.y;
			if (((crossProduct((enemyList[i].x-v1.x),(enemyList[i].y-v1.y),dx,dy) > 0) != (crossProduct((player.x-v1.x),(player.y-v1.y),dx,dy) > 0)) &&
			((crossProduct((v1.x-player.x),(v1.y-player.y),edx,edy) > 0) != (crossProduct((v2.x-player.x),(v2.y-player.y),edx,edy) > 0))) inView = 0;
		}
		
		if (inView) {
			if (enemyRenderCount < maxEnemy)
				enemyRenderList[enemyRenderCount++] = i;
			if (enemyList[i].cooldown == 0) {
				enemyList[i].cooldown = firingCooldown;
				playSound(playerHurt);
				if (player.armor > 0)
					player.armor -= 1;
				else
					player.health -= 1;
			}
		}
	}
	
	if (player.armor < 0)	player.armor = 0;
}
