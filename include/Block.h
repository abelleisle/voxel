#ifndef BLOCK_H
#define BLOCK_H

#include <common.h>

class Chunk;

class Block{
public:
	vec3 loc;
	uint hash;

	std::string name;
	uint id;
	Color color;

	Chunk *inChunk;

	Block();
	Block(vec3 l);
	void update();
};

class Chunk{
public:
	std::unordered_map<uint, Block>block;

	vec3 loc;
	uint hash;

	Chunk(vec3 l);
	Chunk();

	void updateBlocks();
};

class World{
public:

	std::unordered_map<uint, Chunk>chunk;

	World();

	void createChunk(vec3 l);
	void updateChunks();

	Block* blockAt(vec3);
};

//ID
//0 = air

//Block Vertices are stored like this
/*

      8---7
     /|  /|
    5-+-6 |
	| 4-+-3
	|/	|/
	1---2




*/
#endif //BLOCK_H