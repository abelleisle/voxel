#ifndef BLOCK_H
#define BLOCK_H

#include <common.h>

class Block{
public:
	Block();
	void update();

	vec3 loc;
	std::vector<float>verts;
	std::vector<uint>vertOrder;

	std::string name;
	uint id;
	Color color;
};

class Chunk{
public:
	friend class Block;
	Block block[CHUNK_HEIGHT][CHUNK_WIDTH][CHUNK_DEPTH];

	vec3 loc;
	std::vector<float>verts;
	std::vector<uint>vertOrder;


	Chunk(vec3 l);
};

class World{
public:
	std::vector<std::vector<std::vector<Chunk>>>chunk;
	void createChunk(vec3 l){
		chunk.resize(0);
		chunk[0].resize(0);
		chunk[0][0].push_back(l);
	}

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