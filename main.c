#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef int8_t s_number;
typedef uint8_t u_number;

typedef struct {
	s_number x;
	s_number y;
	s_number health;
	s_number rotation;
	s_number facing;
} Weightings;

typedef struct {
	Weightings forwards;
	Weightings eat;
	Weightings turnLeft;
	Weightings turnRight;
} Brain;

typedef struct{
	u_number x;
	u_number y;
	u_number health;
	u_number food;
	Brain brain;
} Creature;

typedef struct {
	u_number width;
	u_number height;
	size_t _creatureCapacity;
	size_t creatureCount;
	Creature** creatures;
} World;

#define CURR_VER 0
typedef struct {
	int version;
	u_number width;
	u_number height;
	size_t creatureCount;
} *WorldFile;

World worldFromFile(WorldFile f)
{
	assert(f->version == CURR_VER);
	World w = {0};
	w.width = f->width;
	w.height = f->height;
	w.creatureCount = f->creatureCount;
	w._creatureCapacity = f->creatureCount;
	w.creatures = malloc(sizeof (w.creatures[0]) * w.creatureCount);
	
	Creature* creaturePtrArr = (Creature*)(f+sizeof (*f));
	for (size_t i = 0; i < w.creatureCount; i++)
	{ w.creatures[i] = &creaturePtrArr[i]; }
	return w;
}

size_t FileFromWorld(World world, WorldFile* f){
	size_t size = sizeof (**f) + ((sizeof world.creatures[0][0]) * world.creatureCount);

	*f = malloc(size);
	if (*f==NULL){assert(false && "not enough ram");}
	(*f)->version = CURR_VER;
	(*f)->width = world.width;
	(*f)->height = world.height;
	(*f)->creatureCount = world.creatureCount;
	Creature* creaturePtrArr = (Creature*)((*f)+sizeof (**f));

	for (size_t i = 0; i < world.creatureCount; i++)
	{ memccpy(&(creaturePtrArr[i]), world.creatures[i], 1, sizeof(Creature)); }

	return size;
}

void* dumpFileMalloc(FILE* fptr){
	fseek(fptr, 0L, SEEK_END);
	size_t sz = ftell(fptr);
	fseek(fptr, 0L, SEEK_SET);

	void* fc = malloc(sz);
	fread(fc,sz,1,fptr);
	return fc;
}

#define CELL_SIZE 10
#define CHANNEL_NUM 4
#define VECTOR2(world, x, y) (((world).width * CELL_SIZE * y) + x)

_Static_assert(CHANNEL_NUM == 4, "Colour expects 4 channels");

typedef union {
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
	uint32_t n;
} Colour;


void renderWorldToImage(char const *filename, World world){
	Colour* data = malloc(world.width * CELL_SIZE * world.height * CELL_SIZE * sizeof data[0]);

	for (size_t i = 0; i < world.creatureCount; i++) {
		for (
			size_t y = CELL_SIZE * world.creatures[i]->y;
			y < (size_t)CELL_SIZE * (world.creatures[i]->y + 1);
			y++
		) {for (
			size_t x = CELL_SIZE * world.creatures[i]->x;
			x < (size_t)CELL_SIZE * (world.creatures[i]->x + 1);
			x++
		) { data[VECTOR2(world, x, y)] = (Colour){.r=255, .g=0, .b=0, .a=255,}; }}
	}


	stbi_write_png(filename, world.width * CELL_SIZE, world.height * CELL_SIZE, CHANNEL_NUM, data, world.width * CELL_SIZE * CHANNEL_NUM);
}

int main(int argc, char** argv){
	assert(argc == 2);
	World w = {
		.width = 15,
		.height = 19,
		.creatureCount = 2,
	};

	w.creatures = malloc(sizeof(w.creatures[0])*2);

	w.creatures[0] = &(Creature){
		.x=2,
		.y=4,
		.health=6,
		.food=8,
	};
	w.creatures[1] = &(Creature){
		.x=10,
		.y=12,
		.health=14,
		.food=15,
	};

	printf("%s", argv[1]);
	renderWorldToImage(argv[1], w);

	return 0;
}
