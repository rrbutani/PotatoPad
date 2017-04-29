// Types.h
// Runs on LM4F120/TM4C123
// Contains all the structs used by PotatoDoom
// Steven Zhu

typedef struct {
	float x;
	float y;
	float z;
	float	angle;	// in radians; 0 is east
	uint16_t health;
} Player;

typedef struct {
	int16_t x;
	int16_t y;
} VERTEX;

typedef struct {
	int16_t startVertex;
	int16_t endVertex;
	int16_t	angle;	// 0000=east, 4000=north, 8000=west, c000=south
	int16_t	linedef;
	uint8_t direction;	// 0 if same as linedef, 1 if opposite
	int16_t offset;	// Offset from start of linedef
} SEG;

typedef struct {
	int16_t vertexStart;
	int16_t vertexEnd;
	int16_t flags;
	int16_t types;
	int16_t tag;		// Trigger that ties to all SECTOR with same tag
	int16_t rightSide;	// Number of right SIDEDEF
	int16_t leftSide;		// Number of left SIDEDEF, -1 if none exist
} LINEDEF;

typedef struct {
	int16_t xOffset;
	int16_t yOffset;
	uint8_t	upper;		// Upper texture name, -1 or 255 for transparent
	uint8_t	lower;		// Lower texture name
	uint8_t	middle;		// Middle/normal texture name
	int16_t	sector;			// Parent sector
} SIDEDEF;

typedef struct {
	int16_t numSeg;		// Total number of segments
	int16_t startSeg;	// Starting segment
} SSECTOR;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t dx;		// length in x of partition line
	int16_t dy;		// length in y of partition line
	uint16_t rightChild;		// right child, could be NODE or SSECTOR
	uint16_t leftChild;		// bit 15 indicate whether child is SSECTOR (1 = True)
} NODE;

// UNUSED
//typedef struct {
//	int16_t floorHeight;
//	int16_t ceilingHeight;
//	uint8_t	floorFlat[8];
//	uint8_t	ceilingFlat[8];
//	int16_t	lightLevel;
//	int16_t	special;
//	int16_t	tag;
//} SECTOR;

typedef struct {
	uint8_t height;
	uint8_t width;
	const unsigned short *texture;
} PATCH;

// Transparent patch
typedef struct {
	uint8_t height;
	uint8_t width;
	const uint8_t *space;		// Number of pixel occupied
	const uint8_t *offset;	// 0 for no offset (ex: gun and HUD)
	const unsigned short *texture;
} TPATCH;
