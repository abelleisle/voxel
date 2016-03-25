#ifndef BLOCK_H
#define BLOCK_H

#include <common.h>
#include <Texture.h>

class Chunk;

enum block_t{
	AIR,
	SOLID
};

enum block_side{
	NEAR,
	FAR,
	RIGHT,
	LEFT,
	TOP,
	BOTTOM
};

class Block{
public:
	vec3 loc;
	uint hash;

	block_t type;

	std::vector<vec3>verts;
	std::vector<Color>colors;
	std::vector<block_side>normals;

	std::string name;
	uint id;
	Color color;

	GLuint texture;

	Chunk *inChunk;

	Block();
	Block(vec3 l);
	void updateFaces();
};

class World;

class Chunk{
public:
	//std::unordered_map<uint, Block>block;
	std::vector<std::vector<std::vector<Block>>>block;

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
