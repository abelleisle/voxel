#ifndef BLOCK_H
#define BLOCK_H

#include <common.hpp>
#include <texture.hpp>
#include <threadpool.hpp>

class Chunk;

enum block_t{
	AIR,
	SOLID
};

//image id
//first coord is where
//second is either top side or bottom
//0 = top
//1 = side
//2 = bottom

class Block{
public:
	vec3 loc;

	block_t type;

	std::vector<vec3>verts;
	std::vector<Color>colors;
	Color color;

	uint id;

	Chunk *inChunk;

	Block();
	Block(vec3 l);
	std::vector<vec3> updateFaces();
};

class World;

class Chunk{
public:
	//std::unordered_map<uint, Block>block;
	std::vector<std::vector<std::vector<Block>>>block;

	GLuint vbo;
	std::vector<vec3>vertex;
	int elements = 0;

	vec3 loc;
	uint hash;

	World *inWorld;

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
	bool blockIsAir(vec3);

	Block generateBlock(vec3 l);
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
