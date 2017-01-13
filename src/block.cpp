#include <block.hpp>

extern GLint attribute_coord;
extern GLint attribute_t_index;
extern GLuint shaderProgram;
extern GLint uniform_mvp;

extern World world;

extern std::vector<GLuint*>buffersToGen;
extern std::vector<Chunk*>chunkPtrs;
extern bool started;

std::mutex mtx;
std::mutex blockMtx;

BlockData blockData;

/*
 *	CONSTRUCTORS
 */

SuperChunk::SuperChunk(vec3 l) : loc(l)
{
	for (int x = 0; x < SUPER_WIDTH; x++) {
		for (int y = 0; y < SUPER_HEIGHT; y++) {
			for (int z = 0; z < SUPER_DEPTH; z++) {
				chunk[x][y][z] = new Chunk(vec3(loc.x + x, loc.y + y, loc.z + z));
			}
		}
	}

	for (int x = 0; x < SUPER_WIDTH; x++) {
		for (int y = 0; y < SUPER_HEIGHT; y++) {
			for (int z = 0; z < SUPER_DEPTH; z++) {
				if(x > 0)
					chunk[x][y][z]->left = chunk[x - 1][y][z];
				if(x < SUPER_WIDTH - 1)
					chunk[x][y][z]->right = chunk[x + 1][y][z];
				if(y > 0)
					chunk[x][y][z]->below = chunk[x][y - 1][z];
				if(y < SUPER_HEIGHT - 1)
					chunk[x][y][z]->above = chunk[x][y + 1][z];
				if(z > 0)
					chunk[x][y][z]->front = chunk[x][y][z - 1];
				if(z < SUPER_DEPTH - 1)
					chunk[x][y][z]->behind = chunk[x][y][z + 1];
			}
		}
	}
	
}

SuperChunk::SuperChunk() : SuperChunk(vec3(0,0,0)) {}

Chunk::Chunk(vec3 l): loc(l)
{
	init = false;
	updated = false;
	fillvbo = false;
	elements = 0;
	highest = 0;
	right = left = above = below = front = behind = 0;

	glGenBuffers(1, &vert_vbo);
}

Chunk::Chunk() : Chunk(vec3(0,0,0)) {}

/*
 *	OTHER FUNCTIONS
 */

void Chunk::generate(uint64_t seed)
{
	std::thread([&]{
	//TODO Seed
	(void)seed;
	for (int y = 0; y < CHUNK_HEIGHT; y++) {
		for (int z = 0; z < CHUNK_DEPTH; z++) {
			for (int x = 0; x < CHUNK_WIDTH; x++) {
				block[x][y][z] = generateBlock(vec3(x+(loc.x*CHUNK_WIDTH),y+(loc.y*CHUNK_HEIGHT),z+(loc.z*CHUNK_DEPTH)));
			}
		}
	}
	updated = true;
	}).detach();
}

void Chunk::updateBlocks()
{
	byte4 vertex[CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT * 18];
	int i = 0;
	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int y = 0; y < CHUNK_HEIGHT; y++) {
			for (int z = 0; z < CHUNK_DEPTH; z++) {
				if (block[x][y][z]) {
					if (y < CHUNK_HEIGHT-1 && !block[x][y+1][z]) {
						vertex[i++] = byte4(x,	y,	z,	block[x][y][z]);
						vertex[i++] = byte4(x+1,y,	z,	block[x][y][z]);
						vertex[i++] = byte4(x+1,y,	z+1,block[x][y][z]);
						
						vertex[i++] = byte4(x+1,y,	z+1,block[x][y][z]);
						vertex[i++] = byte4(x,	y,	z,	block[x][y][z]);
						vertex[i++] = byte4(x,	y,	z+1,block[x][y][z]);
					}
				}
			}
		}
	}
	
	elements = i;

	std::copy(std::begin(vertex), std::end(vertex), std::begin(vertexdata));
	
	updated = false;
	//fillvbo = true;


	//glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
	//glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertexdata, vertexdata, GL_STATIC_DRAW);
	
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);*/
}

int SuperChunk::render(const glm::mat4 &pv)
{
	float ud = 1.0 / 0.0;
	int ux = -1;
	int uy = -1;
	int uz = -1;
	for (int x = 0; x < SUPER_WIDTH; x++) {
		for (int y = 0; y < SUPER_HEIGHT; y++) {
			for (int z = 0; z < SUPER_DEPTH; z++) {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(chunk[x][y][z]->loc.x * CHUNK_WIDTH, chunk[x][y][z]->loc.y * CHUNK_HEIGHT, chunk[x][y][z]->loc.z * CHUNK_DEPTH));
				glm::mat4 mvp = pv * model;

				// Is this chunk on the screen?
				glm::vec4 center = mvp * glm::vec4(CHUNK_WIDTH / 2, CHUNK_WIDTH / 2, CHUNK_DEPTH / 2, 1);

				float d = glm::length(center);
				center.x /= center.w;
				center.y /= center.w;

				// If it is behind the camera, don't bother drawing it
				if(center.z < -CHUNK_DEPTH / 2)
					continue;

				// If it is outside the screen, don't bother drawing it
				if(fabsf(center.x) > 1 + fabsf(CHUNK_HEIGHT * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CHUNK_HEIGHT * 2 / center.w))
					continue;
			
				if(d > CHUNK_RAD * sqrt(pow(CHUNK_WIDTH,2)+pow(CHUNK_DEPTH,2))) {
					chunk[x][y][z]->init = false;
					continue;
				}

				// If this chunk is not initialized, skip it
				if(!chunk[x][y][z]->init) {
					// But if it is the closest to the camera, mark it for initialization
					if(ux < 0 || d < ud) {
						ud = d;
						ux = x;
						uy = y;
						uz = z;
					}
					continue;
				}

				glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

				chunk[x][y][z]->render();
			}
		}
	}
		
	if(ux >= 0) {
		chunk[ux][uy][uz]->generate(seed);
		if(chunk[ux][uy][uz]->left)
			chunk[ux][uy][uz]->left->generate(seed);
		if(chunk[ux][uy][uz]->right)
			chunk[ux][uy][uz]->right->generate(seed);
		if(chunk[ux][uy][uz]->below)
			chunk[ux][uy][uz]->below->generate(seed);
		if(chunk[ux][uy][uz]->above)
			chunk[ux][uy][uz]->above->generate(seed);
		if(chunk[ux][uy][uz]->front)
			chunk[ux][uy][uz]->front->generate(seed);
		if(chunk[ux][uy][uz]->behind)
			chunk[ux][uy][uz]->behind->generate(seed);
		chunk[ux][uy][uz]->init = true;
	}

	return 1;
}

int Chunk::render()
{
	if(!init)
		return 0;

	if (updated) {
		std::thread([&]{updateBlocks();}).detach();
		//updateBlocks();
	}

	if (fillvbo) {
		glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
		glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertexdata, vertexdata, GL_STATIC_DRAW);
		fillvbo = false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
	glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertexdata, vertexdata, GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
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

const int SEA = 12;

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

/*
 * DESTRUCTORS
 */

SuperChunk::~SuperChunk()
{
	//burn
}

Chunk::~Chunk()
{
	glDeleteBuffers(1, &vert_vbo);
}
