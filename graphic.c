#include <stdint.h>
#include <math.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "SD.h"

void EnableInterrupts(void);

#define W 320 
#define H 216		// 240 - 24 = 216

#define hfov (0.73f*H)
#define vfov (0.2f*H)

struct sector_t{
	struct xy {int x, y;} *vertex;
	int floor, ceiling, numVertices;
	struct sector_t *neighbor;
} *sectors = 0;
struct player_t{
	struct xyz {float x, y, z;} location, velocity;
	struct sector_t *sector;
	float angle, angleSin, angleCos;
} player;
unsigned int numSectors = 0;

void drawPixel(int x, int y, int color){};

//void load(void) {
//	numSectors = 2;
//	sectors = realloc(sectors, numSectors * sizeof(*sectors));
//	struct xy cord[] = {{0,0},{0,10},{10,10},{10,0}};
//	struct xy cord1[] = {{10,0},{10,10},{25,5}};
//	struct sector_t* sect = sectors;
//	sect->vertex = cord;
//	sect->floor = 0;
//	sect->ceiling = 10;
//	sect->neighbor = sectors+sizeof(*sectors);
//	sect->numVertices = 4;
//	sect++;
//	sect->vertex = cord1;
//	sect->floor = 0;
//	sect->ceiling = 10;
//	sect->neighbor = sectors;
//	sect->numVertices = 3;
//}

static void drawVertCol(int x, int y1, int y2, int top, int middle, int bottom) {
	if (y1 == y2) {
		drawPixel(x, y1, middle);
	} else {
		drawPixel(x,y1,top);
		for (int i = y1+1; i < y2; i++) {
			drawPixel(x,i,middle);
		}
	
		drawPixel(x,y2,bottom);
	}
}

#define MAXQUEUE	32
#define nearX 0.25
#define farX
#define nearY
#define farY	
static void drawScreen() {
	int yTop[W], yBottom[W];
	int rendered[numSectors];
	float	pcos = player.angleCos, psin = player.angleSin;
	for (int s = 0; s < numSectors; s++) {
		for (int v = 0; v < sectors[s].numVertices - 1; v++) {
		// Loop through all vertices
			float vx1 = sectors[s].vertex[v].x - player.location.x,
				vx2 = sectors[s].vertex[v+1].x - player.location.x,
				vy1 = sectors[s].vertex[v].y - player.location.y,
				vy2 = sectors[s].vertex[v+1].y - player.location.y;
			// Transform to player view
			float tx1 = vx1 * pcos - vy1 * psin, 
				ty1 = vy1 * pcos + vx1 * psin,
				tx2 = vx2 * pcos - vy2 * psin, 
				ty2 = vy2 * pcos + vx2 * psin;
			if (tx1 < 0 && tx2 < 0) continue;		// Completely behind player
//			if (tx1 < 0 || tx2 < 0) {						// Partially behind player
//				float ix = (tx1*tx2)*(tx3-tx);
//			}
		}
	}
}

int main(void){
	PLL_Init(); 		// Set clock to 80MHz
	EnableInterrupts();
	loadData();
	player.location = (struct xyz){2,5,0};
	player.velocity = (struct xyz){0,0,0};
	player.angle = 0;
	player.angleCos = 0;
	player.angleSin = 1;
	player.sector = sectors;
	
	while(1) {
		drawScreen();
	}
}
