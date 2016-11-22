#ifndef BLOCK_H
#define BLOCK_H

#include <stdexcept>
#include <common.hpp>
#include <texture.hpp>
#include <threadpool.hpp>

#define BOTTOM 	1
#define TOP 	2
#define RIGHT 	4
#define LEFT 	8
#define NEAR 	16
#define FAR 	32

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

/*std::unordered_map<std::string, uint8_t> block_map;

uint8_t addBlockID(std::string s)
{
	block_map.emplace(s,block_map.size());
	return (uint8_t)block_map.size()-1;	
}

uint8_t addBlockIDWithID(std::string s, uint8_t id)
{
	block_map.emplace(s,id);
	return (uint8_t)id;	
}

uint8_t getBlockID(std::string s)
{
	uint8_t id;
	try {
		id = block_map.at(s);
	} catch (std::out_of_range &oor) {
		return 0;
	}
	return id;
}*/

struct BlockData {
	uint8_t air = 0;
	uint8_t stone = 1;
	uint8_t cobblestone = 2;
	uint8_t dirt = 3;
	uint8_t grass = 4;
	uint8_t water = 5; 	
} blockData;

//image id
//first coord is where
//second is either top side or bottom
//0 = top
//1 = side
//2 = bottom

class block_sides{
public:
	std::vector<vec2> sides;
	std::vector<vec2> top;
	std::vector<vec2> bottom;

	block_sides(vec2 s, vec2 t, vec2 b) {
		vec2 mult = vec2(1.0f/blockBlockx,1.0f/blockBlocky);
		sides.push_back(vec2(s.x * mult.x, s.y * mult.y));
		sides.push_back(vec2((s.x + 1) * mult.x, s.y * mult.y));
		sides.push_back(vec2((s.x + 1) * mult.x, (s.y + 1) * mult.y));
		sides.push_back(vec2(s.x * mult.x, (s.y + 1) * mult.y));

		top.push_back(vec2(t.x * mult.x, t.y * mult.y));
		top.push_back(vec2((t.x + 1) * mult.x, t.y * mult.y));
		top.push_back(vec2((t.x + 1) * mult.x, (t.y + 1) * mult.y));
		top.push_back(vec2(t.x * mult.x, (t.y + 1) * mult.y));

		bottom.push_back(vec2(b.x * mult.x, b.y * mult.y));
		bottom.push_back(vec2((b.x + 1) * mult.x, b.y * mult.y));
		bottom.push_back(vec2((b.x + 1) * mult.x, (b.y + 1) * mult.y));
		bottom.push_back(vec2(b.x * mult.x, (b.y + 1) * mult.y));
	}
	block_sides() {}
};// grass = block_s(vec2(0,1),vec2(0,2),vec2(0,0)), dirt = block_s(vec2(0,0),vec2(0,0),vec2(0,0));

class Block{
public:
	vec3 loc;

	block_t type;
	block_sides side;

	uint id;
	uint edge;

	World *worldIn;
	Chunk *inChunk;

	Block();
	Block(vec3 l);
	~Block();
	std::vector<std::pair<vec3,vec2>>updateFaces(bool child);
};


class Chunk{
public:
	//std::unordered_map<uint, Block>block;
	uint8_t new_block[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
	std::vector<std::vector<std::vector<Block>>>block;

	GLuint vert_vbo, vert_vbo_water;
	GLuint tex_vbo, tex_vbo_water;


	std::vector<vec3>vertex;
	std::vector<vec2>tex_coord;

	std::vector<vec3>vertex_water;
	std::vector<vec2>tex_coord_water;

	int elements = 0, elements_water = 0;
	int highest = 0;
	bool canRender = false;

	vec3 loc;
	long long hash;

	World *inWorld;

	Chunk(vec3 l,World *in);
	Chunk();
	~Chunk();
	int render();
	int renderL();

	void updateBlocks();
};

class World{
public:

	std::unordered_map<long long, Chunk>chunk;

	World();

	void createChunk(vec3 l);
	void updateChunks();
	void updateChunk(vec3 l);

	Block* blockAt(vec3 l);
	Chunk* chunkAt(vec3 l);
	bool blockIsAir(vec3 l);

	uint8_t generateBlock(vec3 l);
	std::pair<block_t, block_sides> generateBlock(vec3 l, Chunk *ptr);
	float noise2D(float x, float y, int octaves, float persistance);

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
