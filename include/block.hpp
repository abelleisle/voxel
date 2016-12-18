#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <stdexcept>
#include <common.hpp>
#include <texture.hpp>
#include <threadpool.hpp>

const float blockBlockx = 5.0f;
const float blockBlocky = 3.0f;

class Chunk;
class World;

enum block_t{
	AIR,
	SOLID,
	GLASS,
	LIQUID
};

struct byte4 {
	uint8_t x, y, z, w;
	byte4() {}
	byte4(uint8_t x, uint8_t y, uint8_t z, uint8_t w): x(x), y(y), z(z), w(w) {}
};

// generation functions
uint8_t generateBlock(vec3 l);
float noise2D(float x, float y, int octaves, float persistance);

struct BlockData {
	uint8_t air = 0;
	uint8_t stone = 1;
	uint8_t cobblestone = 2;
	uint8_t dirt = 3;
	uint8_t grass = 4;
	uint8_t water = 5; 	
};

//image id
//first coord is where
//second is either top side or bottom
//0 = top
//1 = side
//2 = bottom

class Chunk{
public:
	uint8_t block[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
	Chunk *left, *right, *above, *below, *front, *behind; 
	byte4 vertexdata[CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT * 18];
	vec3 loc;

	GLuint vert_vbo;

	bool init;
	bool updated;
	bool fillvbo;
	int elements;
	int highest;

	void generate(uint64_t seed);
	void updateBlocks();

	int render();

	Chunk();
	Chunk(vec3);
	~Chunk();
};

class SuperChunk{
public:
	Chunk *chunk[SUPER_WIDTH][SUPER_HEIGHT][SUPER_DEPTH];
	vec3 loc;
	uint64_t seed = 0;

	int render(const glm::mat4 &pv);

	SuperChunk();
	SuperChunk(vec3);
	~SuperChunk();
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
