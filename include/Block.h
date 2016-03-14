#ifndef BLOCK_H
#define BLOCK_H

#include <common.h>

class Chunk;

class Block{
public:
	Block();
	void update();

	vec3 loc;
	//std::vector<float>verts;
	//std::vector<uint>vertOrder;

	std::string name;
	uint id;
	Color color;

	Chunk *inChunk;
};

class Chunk{
public:
	friend class Block;

	Block block[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_DEPTH];

	vec3 loc;
	std::vector<float>verts;
	std::vector<uint>vertOrder;
	std::vector<uint>vertOrderList;


	Chunk(vec3 l);
	Chunk();

	void updateBlocks();
};

class World{
public:
	World();

	std::vector<std::vector<std::vector<Chunk>>>chunk;
	std::vector<std::vector<std::vector<Chunk>>>loadedChunks;

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