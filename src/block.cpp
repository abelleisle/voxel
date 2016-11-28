#include <block.hpp>

extern GLint attribute_coord;
extern GLint attribute_t_index;
extern GLuint shaderProgram;

extern World world;

extern std::vector<GLuint*>buffersToGen;
extern std::vector<Chunk*>chunkPtrs;
extern bool started;

std::mutex mtx;
std::mutex blockMtx;

BlockData blockData;

Chunk::Chunk(vec3 l): loc(l)
{
	//glGenBuffers(1,&vert_vbo);

	for (int y = 0; y < CHUNK_HEIGHT; y++) {
		for (int z = 0; z < CHUNK_DEPTH; z++) {
			for (int x = 0; x < CHUNK_WIDTH; x++) {
				block[x][y][z] = generateBlock(vec3(x+loc.x,y+loc.y,z+loc.z));
			}
		}
	}
}

Chunk::Chunk() : Chunk(vec3(0,0,0)) {}

void Chunk::updateBlocks()
{
	byte4 vertex[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];

	/*glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(vec3), &vertex[0], GL_STATIC_DRAW);

	canRender = true;
*/

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);*/
}

int Chunk::render()
{
	if (!canRender)return 0;

	glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(vec3), &vertex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, elements);

	return 1;
}

float noise2D(float x, float y, int octaves, float persistance)
{
	float sum = 0;
	float strength = 1.0;
	float scale = 1.0;
	for(int i = 0; i < octaves; i++){
		sum += strength * glm::simplex(glm::vec2(x, y) * scale);
		scale *= 2.0;
		strength *= persistance;
	}
	return sum;
}

const int SEA = 64;

uint8_t generateBlock(vec3 l)
{
	uint8_t id;
	block_t type;

	// determine the height of the blocks
	float n = noise2D((l.x) / 256.0, (l.z) / 256.0, 3, 0.8) * 4;
	int h = n * 2;
	h += SEA;

	// if the current block is less than the height described
	// we will make it solid
	if (l.y <= h) {
		type = SOLID;
		if (l.y == h) {
			id = blockData.grass; //b.second = block_sides(vec2(0,1),vec2(0,2),vec2(0,0)); //grass
		} else {
			id = blockData.dirt; //b.second = block_sides(vec2(0,0),vec2(0,0),vec2(0,0)); //dirt
		}
	} else {
		type = AIR;
	}

	// if a block is air, and it's less than the sea level
	// we make it the sea
	if (type == AIR && l.y <= SEA) {
		type = LIQUID;
		id = blockData.water; //b.second = block_sides(vec2(3,2),vec2(3,2),vec2(3,2)); //water
	}

	// if we gave a chunk, then we update the highest block
	// in the chunk, this will make chunk update times faster
	//if (ptr)
	//	if (l.y > ptr->highest && b.first != AIR)ptr->highest = l.y;

	return id;
}

Chunk::~Chunk()
{
	glDeleteBuffers(1, &vert_vbo);
}
