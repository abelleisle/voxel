#ifndef BLOCK_H
#define BLOCK_H

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

	block_sides(vec2 s, vec2 t, vec2 b){
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
	block_sides(){}
};// grass = block_s(vec2(0,1),vec2(0,2),vec2(0,0)), dirt = block_s(vec2(0,0),vec2(0,0),vec2(0,0));

class Block{
public:
	vec3 loc;

	block_t type;
	block_sides side;

	uint id;

	World *worldIn;

	Block();
	Block(vec3 l);
	std::vector<std::pair<vec3,vec2>>updateFaces();
};


class Chunk{
public:
	//std::unordered_map<uint, Block>block;
	std::vector<std::vector<std::vector<Block>>>block;

	GLuint vert_vbo, vert_vbo_water;
	GLuint tex_vbo, tex_vbo_water;


	std::vector<vec3>vertex;
	std::vector<vec2>tex_coord;

	std::vector<vec3>vertex_water;
	std::vector<vec2>tex_coord_water;

	int elements = 0, elements_water = 0;

	vec3 loc;
	unsigned long long hash;

	World *inWorld;

	Chunk(vec3 l,World *in);
	Chunk();
	void render();
	void renderL();

	void updateBlocks();
};

class World{
public:

	std::unordered_map<unsigned long long, Chunk>chunk;

	World();

	void createChunk(vec3 l);
	void updateChunks();
	void updateChunk(vec3 l);

	Block* blockAt(vec3 l);
	Chunk* chunkAt(vec3 l);
	bool blockIsAir(vec3 l);

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
