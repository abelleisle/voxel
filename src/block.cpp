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

float textureloc(float id) {
	if (id < 0.0f) { //sides
		uint8_t sides[] = {0, 1, 2, 3, 15, 5, 6, 7};
		return -sides[(uint)-id];
	} else { //top and bottom
		uint8_t tops[] = {0, 1, 2, 3, 4, 5, 14, 7};
		return tops[(uint)id];
	}
	return 0;
}

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
	elements = liquid_elements = 0;
	highest = 0;
	right = left = above = below = front = behind = 0;

	glGenBuffers(1, &vert_vbo);
	glGenBuffers(1, &liquid_vert_vbo);
}

Chunk::Chunk() : Chunk(vec3(0,0,0)) {}

/*
 *	OTHER FUNCTIONS
 */

const int SEA = 32;

void Chunk::generate(uint64_t seed)
{
	//std::thread([&]{
	//TODO Seed
	(void)seed;
	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int z = 0; z < CHUNK_DEPTH; z++) {
				
				// determine the height of the blocks
				float n = noise2D((x + loc.x * CHUNK_WIDTH) / 256.0, (z + loc.z * CHUNK_DEPTH) / 256.0, 2, 1.2) * 4;
				int h = n * 2;
				h += (SEA * 1.15);
				
				float dh = rand()%5 + 3;

				for (int y = CHUNK_HEIGHT - 1; y >= 0; y--) {
					int wy = y + loc.y * CHUNK_HEIGHT;

					// if the current block is ss than the height described
					// we will make it solid 
					if (wy <= h) {
						if (wy == h) {
							block[x][y][z] = blockData.grass; //b.second = block_sblock[x][y][z]es(vec2(0,1),vec2(0,2),vec2(0,0)); //grass
						} else if (wy <= h && wy >= h - dh){
							block[x][y][z] = blockData.dirt; //b.second = block_sblock[x][y][z]es(vec2(0,0),vec2(0,0),vec2(0,0)); //dirt
						} else {
							block[x][y][z] = blockData.stone;
						}
					} else if (wy == h+1 && wy - SEA >= 8) {
						int t = rand() % 800;
						if (t == 65) {
							int th = 4 + (rand() % 4);
							int lp;
							for (lp = 0; lp <= th; lp++) {
								block[x][y+lp][z] = blockData.oakLog;
							}
							// Leaves
							for(int ix = -3; ix <= 3; ix++) {
								for(int iy = -3; iy <= 3; iy++) {
									for(int iz = -3; iz <= 3; iz++) {
										int td = (ix * ix) + (iy * iy) + (iz * iz);
										if(td < 8 + (rand() & 1) && !get(x + ix, wy + lp + iy, z + iz)) {
											set(ix + x, iy + lp + y, iz + z, blockData.leaves);
										}
									}
								}
							}
						}
					}

					// if a block is air, and it's s than the sea vel
					// we make it the sea
					if (!block[x][y][z] && wy <= SEA) {
						block[x][y][z] = blockData.water; //b.second = block_sblock[x][y][z]es(vec2(3,2),vec2(3,2),vec2(3,2)); //water
					}
				}
			}
		}
	updated = true;
	//}).detach();
}

uint8_t Chunk::set(int x, int y, int z, uint8_t id){
	if (x < 0)
		return left ? left->set(x + CHUNK_WIDTH, y, z, id) : 0;
	if (x >= CHUNK_WIDTH)
		return right ? right->set(x - CHUNK_WIDTH, y, z, id) : 0;
	if (y < 0)
		return below ? below->set(x, y + CHUNK_HEIGHT, z, id) : 0;
	if (y >= CHUNK_HEIGHT)
		return above ? above->set(x, y - CHUNK_HEIGHT, z, id) : 0;	
	if (z < 0)
		return front ? front->set(x, y, z + CHUNK_DEPTH, id) : 0;
	if (z >= CHUNK_DEPTH)
		return behind ? behind->set(x, y, z - CHUNK_DEPTH, id) : 0;	

	block[x][y][z] = id;
	return id;
}

uint8_t Chunk::get(int x, int y, int z) const{
	if (x < 0)
		return left ? left->get(x + CHUNK_WIDTH, y, z) : 0;
	if (x >= CHUNK_WIDTH)
		return right ? right->get(x - CHUNK_WIDTH, y, z) : 0;
	if (y < 0)
		return below ? below->get(x, y + CHUNK_HEIGHT, z) : 0;
	if (y >= CHUNK_HEIGHT)
		return above ? above->get(x, y - CHUNK_HEIGHT, z) : 0;	
	if (z < 0)
		return front ? front->get(x, y, z + CHUNK_DEPTH) : 0;
	if (z >= CHUNK_DEPTH)
		return behind ? behind->get(x, y, z - CHUNK_DEPTH) : 0;	
	
	return block[x][y][z];
}

bool Chunk::blocked(int x, int y, int z, int xt, int yt, int zt) const{
	if (!get(x, y, z)) // air is always blocked
		return true;

	if (!get(xt, yt, zt)) // if the second block is air of course this one won't be blocked
		return false;
	
	if (get(xt, yt, zt) == blockData.leaves) //leaves don't block anything 
		return false;

	if (get(xt, yt, zt) != blockData.water)
		return true;
	
	return get(xt, yt, zt) == get(x, y, z);
}

void Chunk::updateBlocks()
{
	std::vector<byte4> vertex(CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT * 18);
	std::vector<byte4> liquid_vertex(CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT * 18);
	int i = 0, l = 0;
	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int y = 0; y < CHUNK_HEIGHT; y++) {
			for (int z = 0; z < CHUNK_DEPTH; z++) {
				if (block[x][y][z] != blockData.water) {

					if (!blocked(x, y, z, x-1, y, z)) {
						vertex[i++] = byte4(x,y,	z,	textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x,y,	z+1,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x,y+1,	z,	textureloc(-block[x][y][z]));
						
						vertex[i++] = byte4(x,y+1,z,	textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x,y,  z+1,	textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x,y+1,z+1,	textureloc(-block[x][y][z]));
					}

					if (!blocked(x, y, z, x+1, y, z)) {
						vertex[i++] = byte4(x+1,y,	z,	textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y+1,z,	textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y,	z+1,textureloc(-block[x][y][z]));
						
						vertex[i++] = byte4(x+1,y+1,z,	textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y+1,z+1,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y,	z+1,textureloc(-block[x][y][z]));
					}
					
					if (!blocked(x, y, z, x, y-1, z)) {
						vertex[i++] = byte4(x,	y,	z,	textureloc(block[x][y][z]));
						vertex[i++] = byte4(x+1,y,	z,	textureloc(block[x][y][z]));
						vertex[i++] = byte4(x,	y,	z+1,textureloc(block[x][y][z]));
						
						vertex[i++] = byte4(x+1,y,	z,	textureloc(block[x][y][z]));
						vertex[i++] = byte4(x+1,y,	z+1,textureloc(block[x][y][z]));
						vertex[i++] = byte4(x,	y,	z+1,textureloc(block[x][y][z]));
					}
					
					if (!blocked(x, y, z, x, y+1, z)) {
						vertex[i++] = byte4(x,	y+1,z,	textureloc(block[x][y][z]));
						vertex[i++] = byte4(x,	y+1,z+1,textureloc(block[x][y][z]));
						vertex[i++] = byte4(x+1,y+1,z,	textureloc(block[x][y][z]));
						
						vertex[i++] = byte4(x+1,y+1,z,	textureloc(block[x][y][z]));
						vertex[i++] = byte4(x,	y+1,z+1,textureloc(block[x][y][z]));
						vertex[i++] = byte4(x+1,y+1,z+1,textureloc(block[x][y][z]));
					}
					
					if (!blocked(x, y, z, x, y, z-1)) {
						vertex[i++] = byte4(x,	y,	z,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x,	y+1,z,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y,	z,textureloc(-block[x][y][z]));
						
						vertex[i++] = byte4(x,	y+1,z,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y+1,z,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y,	z,textureloc(-block[x][y][z]));
					}
					
					if (!blocked(x, y, z, x, y, z+1)) {
						vertex[i++] = byte4(x,	y,	z+1,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y,	z+1,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x,	y+1,z+1,textureloc(-block[x][y][z]));
						
						vertex[i++] = byte4(x,	y+1,z+1,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y,	z+1,textureloc(-block[x][y][z]));
						vertex[i++] = byte4(x+1,y+1,z+1,textureloc(-block[x][y][z]));
					}
				} 
				else if (block[x][y][z] == blockData.water) {
					if (!blocked(x, y, z, x-1, y, z)) {
						liquid_vertex[l++] = byte4(x,y,	z,	textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x,y,	z+1,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x,y+1,	z,	textureloc(-block[x][y][z]));
						
						liquid_vertex[l++] = byte4(x,y+1,z,	textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x,y,  z+1,	textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x,y+1,z+1,	textureloc(-block[x][y][z]));
					}

					if (!blocked(x, y, z, x+1, y, z)) {
						liquid_vertex[l++] = byte4(x+1,y,	z,	textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y+1,z,	textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y,	z+1,textureloc(-block[x][y][z]));
						
						liquid_vertex[l++] = byte4(x+1,y+1,z,	textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y+1,z+1,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y,	z+1,textureloc(-block[x][y][z]));
					}
					
					if (!blocked(x, y, z, x, y-1, z)) {
						liquid_vertex[l++] = byte4(x,	y,	z,	textureloc(block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y,	z,	textureloc(block[x][y][z]));
						liquid_vertex[l++] = byte4(x,	y,	z+1,textureloc(block[x][y][z]));
						
						liquid_vertex[l++] = byte4(x+1,y,	z,	textureloc(block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y,	z+1,textureloc(block[x][y][z]));
						liquid_vertex[l++] = byte4(x,	y,	z+1,textureloc(block[x][y][z]));
					}
					
					if (!blocked(x, y, z, x, y+1, z)) {
						liquid_vertex[l++] = byte4(x,	y+1,z,	textureloc(block[x][y][z]));
						liquid_vertex[l++] = byte4(x,	y+1,z+1,textureloc(block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y+1,z,	textureloc(block[x][y][z]));
						
						liquid_vertex[l++] = byte4(x+1,y+1,z,	textureloc(block[x][y][z]));
						liquid_vertex[l++] = byte4(x,	y+1,z+1,textureloc(block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y+1,z+1,textureloc(block[x][y][z]));
					}
					
					if (!blocked(x, y, z, x, y, z-1)) {
						liquid_vertex[l++] = byte4(x,	y,	z,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x,	y+1,z,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y,	z,textureloc(-block[x][y][z]));
						
						liquid_vertex[l++] = byte4(x,	y+1,z,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y+1,z,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y,	z,textureloc(-block[x][y][z]));
					}
					
					if (!blocked(x, y, z, x, y, z+1)) {
						liquid_vertex[l++] = byte4(x,	y,	z+1,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y,	z+1,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x,	y+1,z+1,textureloc(-block[x][y][z]));
						
						liquid_vertex[l++] = byte4(x,	y+1,z+1,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y,	z+1,textureloc(-block[x][y][z]));
						liquid_vertex[l++] = byte4(x+1,y+1,z+1,textureloc(-block[x][y][z]));
					}
				}
			}
		}
	}
	
	elements = i;
	liquid_elements = l;
	if (!elements && !liquid_elements)
		return;

	if (elements) {
		vertexdata = new byte4[elements];
		std::copy(std::begin(vertex), std::begin(vertex) + elements, &vertexdata[0]);
	}
	if (liquid_elements) {
		liquid_vertexdata = new byte4[liquid_elements];
		std::copy(std::begin(liquid_vertex), std::begin(liquid_vertex) + liquid_elements, &liquid_vertexdata[0]);
	}
	updated = false;
	fillvbo = true;

	//glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
	//glBufferData(GL_ARRAY_BUFFER, elements * sizeof(byte4), &vertexdata[0], GL_STATIC_DRAW);
	
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
	glm::mat4 model, mvp;
	for (int x = 0; x < SUPER_WIDTH; x++) {
		for (int y = 0; y < SUPER_HEIGHT; y++) {
			for (int z = 0; z < SUPER_DEPTH; z++) {
				chunk[x][y][z]->renderLiquids = false;
				model = glm::translate(glm::mat4(1.0f), glm::vec3(chunk[x][y][z]->loc.x * CHUNK_WIDTH, chunk[x][y][z]->loc.y * CHUNK_HEIGHT, chunk[x][y][z]->loc.z * CHUNK_DEPTH));
				mvp = pv * model;

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
				chunk[x][y][z]->renderLiquids = chunk[x][y][z]->render();
			}
		}
	}

	for (int x = 0; x < SUPER_WIDTH; x++) {
		for (int y = 0; y < SUPER_HEIGHT; y++) {
			for (int z = 0; z < SUPER_DEPTH; z++) {
				if (chunk[x][y][z]->renderLiquids) {
					model = glm::translate(glm::mat4(1.0f), glm::vec3(chunk[x][y][z]->loc.x * CHUNK_WIDTH, chunk[x][y][z]->loc.y * CHUNK_HEIGHT, chunk[x][y][z]->loc.z * CHUNK_DEPTH));
					mvp = pv * model;
					glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
					chunk[x][y][z]->renderLiquid();
				}
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

void glError()
{
	// check OpenGL error
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << err << std::endl;
	}
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
		glBindBuffer(GL_ARRAY_BUFFER, liquid_vert_vbo);
		glBufferData(GL_ARRAY_BUFFER, liquid_elements * sizeof(byte4), &liquid_vertexdata[0], GL_DYNAMIC_DRAW);	
		glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
		//glDrawArrays(GL_TRIANGLES, 0, liquid_elements);
		
		glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
		glBufferData(GL_ARRAY_BUFFER, elements * sizeof(byte4), &vertexdata[0], GL_DYNAMIC_DRAW);	
		glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, elements);
		
	} else {
		if (fillvbo) {
			fillvbo = false;
		} else {
			glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
			glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
			glDrawArrays(GL_TRIANGLES, 0, elements);
		}
	}
	
	return 1;
}

int Chunk::renderLiquid()
{
	glBindBuffer(GL_ARRAY_BUFFER, liquid_vert_vbo);
	glBufferData(GL_ARRAY_BUFFER, liquid_elements * sizeof(byte4), &liquid_vertexdata[0], GL_DYNAMIC_DRAW);	
	glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, liquid_elements);

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

uint8_t generateBlock(vec3 l)
{
	uint8_t id = blockData.air;

	// determine the height of the blocks
	float n = noise2D((l.x) / 256.0, (l.z) / 256.0, 3, 0.8) * 4;
	int h = n * 2;
	h += SEA;

	// if the current block is less than the height described
	// we will make it solid
	if (l.y <= h) {
		if (l.y == h) {
			id = blockData.grass; //b.second = block_sides(vec2(0,1),vec2(0,2),vec2(0,0)); //grass
		} else {
			id = blockData.dirt; //b.second = block_sides(vec2(0,0),vec2(0,0),vec2(0,0)); //dirt
		}
	}

	// if a block is air, and it's less than the sea level
	// we make it the sea
	if (id == blockData.air && l.y <= SEA) {
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
	glDeleteBuffers(1, &liquid_vert_vbo);
}
