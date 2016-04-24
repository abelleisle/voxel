#include <block.hpp>

extern GLint attribute_coord;
extern GLint attribute_t_index;

extern World world;

extern std::vector<GLuint*>buffersToGen;
extern std::vector<Chunk*>chunkPtrs;
extern bool started;

std::mutex mtx;
std::mutex blockMtx;
ThreadPool thr(50);

Block::Block()
{

}

Block::Block(vec3 l)
{
	loc = l;
}

Block* World::blockAt(vec3 l)
{
	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	long long hash = vec3Hash(buf);
	Chunk *chunkPtr = nullptr;

	try{
		chunkPtr = &chunk.at(hash);
	}catch(const std::out_of_range& oor) {
		return nullptr;
	}

	return &chunkPtr->block[l.x-buf.x][l.y-buf.y][l.z-buf.z];
}

Chunk* World::chunkAt(vec3 l)
{
	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;
	//std::cout << buf.x << ", " << buf.y << ", " << buf.z << std::endl;

	if (buf.y < 0)return nullptr;

	long long hash = vec3Hash(buf);
	Chunk *chunkPtr = nullptr;

	try{
		chunkPtr = &chunk.at(hash);
	}catch(const std::out_of_range& oor) {
		return nullptr;
	}

	return chunkPtr;
}

bool World::blockIsAir(vec3 l)
{
	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	long long hash = vec3Hash(buf);
	Chunk *chunkPtr = nullptr;

	try{
		chunkPtr = &chunk.at(hash);
	}catch(const std::out_of_range& oor) {
		return true;
	}
	block_t ty = chunkPtr->block[l.x-buf.x][l.y-buf.y][l.z-buf.z].type;

	if (ty == AIR || ty == LIQUID || ty == GLASS)
		return true;

	return false;
}

std::vector<std::pair<vec3,vec2>> Block::updateFaces(bool child)
{
	// if the block is air, return no verticies
	if (this->type == AIR)return std::vector<std::pair<vec3,vec2>>();

	// TODO
	if (child)std::cout << child << std::endl;

	// our variable for storign world and texture coordinates
 	std::vector<std::pair<vec3,vec2>> verts;

	// the pointers for each side of the block
	Block 	*bRight   	= nullptr,
			*bLeft		= nullptr,
			*bTop 		= nullptr,
			*bBottom 	= nullptr,
			*bNear 		= nullptr,
			*bFar 		= nullptr;

	// chunk pointer we use to determine if a chunk exists
	Chunk 	*cBuf 		= nullptr;

	if (edge & 1) {
		cBuf = worldIn->chunkAt(vec3(loc.x,loc.y-1,loc.z));
		if (cBuf != nullptr)
			bBottom = &cBuf->block[(loc.x-inChunk->loc.x)][CHUNK_HEIGHT-1][(loc.z-inChunk->loc.z)];
	} else {
		bBottom = &inChunk->block[(loc.x-inChunk->loc.x)][(loc.y-inChunk->loc.y)-1][(loc.z-inChunk->loc.z)];
	}

	if (edge & 2) {
		cBuf = worldIn->chunkAt(vec3(loc.x,loc.y+1,loc.z));
		if (cBuf != nullptr)
			bTop = &cBuf->block[(loc.x-inChunk->loc.x)][0][(loc.z-inChunk->loc.z)];
	} else {
		bTop = &inChunk->block[(loc.x-inChunk->loc.x)][(loc.y-inChunk->loc.y)+1][(loc.z-inChunk->loc.z)];
	}

	if (edge & 4) {
		cBuf = worldIn->chunkAt(vec3(loc.x+1,loc.y,loc.z));
		if (cBuf != nullptr)
			bRight = &cBuf->block[0][(loc.y-inChunk->loc.y)][(loc.z-inChunk->loc.z)];
	} else {
		bRight = &inChunk->block[(loc.x-inChunk->loc.x)+1][(loc.y-inChunk->loc.y)][(loc.z-inChunk->loc.z)];
	}

	if (edge & 8) {
		cBuf = worldIn->chunkAt(vec3(loc.x-1,loc.y,loc.z));
		if (cBuf != nullptr)
			bLeft = &cBuf->block[CHUNK_WIDTH-1][(loc.y-inChunk->loc.y)][(loc.z-inChunk->loc.z)];
	} else {
		bLeft = &inChunk->block[(loc.x-inChunk->loc.x)-1][(loc.y-inChunk->loc.y)][(loc.z-inChunk->loc.z)];
	}

	if (edge & 16) {
		cBuf = worldIn->chunkAt(vec3(loc.x,loc.y,loc.z-1));
		if (cBuf != nullptr)
			bNear = &cBuf->block[(loc.x-inChunk->loc.x)][(loc.y-inChunk->loc.y)][CHUNK_DEPTH-1];
	} else {
		bNear = &inChunk->block[(loc.x-inChunk->loc.x)][(loc.y-inChunk->loc.y)][(loc.z-inChunk->loc.z)-1];
	}


	if (edge & 32) {
		cBuf = worldIn->chunkAt(vec3(loc.x,loc.y,loc.z+1));
		if (cBuf != nullptr)
			bFar = &cBuf->block[(loc.x-inChunk->loc.x)][(loc.y-inChunk->loc.y)][0];
	} else {
		bFar = &inChunk->block[(loc.x-inChunk->loc.x)][(loc.y-inChunk->loc.y)][(loc.z-inChunk->loc.z)+1];
	}

	if (this->type == LIQUID || this->type == GLASS) {
		Block* ptr;
		//RIGHT
		ptr = bRight;
		if (ptr != nullptr && ptr->type == AIR) {
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));
		} else {
		}

		//LEFT
		ptr = bLeft;
		if (ptr != nullptr && ptr->type == AIR) {
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));
		} else {
		}

		//TOP
		ptr = bTop;
		if (ptr != nullptr && ptr->type == AIR) {
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.top[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z+1), side.top[3]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
		} else {
		}

		//BOTTOM
		ptr = bBottom;
		if (ptr != nullptr && ptr->type == AIR) {
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.bottom[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z+1), side.bottom[3]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));
		} else {
		}

		//NEAR
		ptr = bNear;
		if (ptr != nullptr && ptr->type == AIR) {
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));
		} else {
		}

		//FAR
		ptr = bFar;
		if (ptr != nullptr && ptr->type == AIR) {
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));
		} else {
		}
	}
	else{
		Block* ptr;
		//RIGHT
		ptr = bRight;
		if (ptr != nullptr && (ptr->type == AIR || ptr->type == LIQUID || ptr->type == GLASS)) {
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));
		} else {
		}

		//LEFT
		ptr = bLeft;
		if (ptr != nullptr && (ptr->type == AIR || ptr->type == LIQUID || ptr->type == GLASS)) {
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));
		} else {
		}

		//TOP
		ptr = bTop;
		if (ptr != nullptr && (ptr->type == AIR || ptr->type == LIQUID || ptr->type == GLASS)) {
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.top[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z+1), side.top[3]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
		} else {
		}

		//BOTTOM
		ptr = bBottom;
		if (ptr != nullptr && (ptr->type == AIR || ptr->type == LIQUID || ptr->type == GLASS)) {
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.bottom[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z+1), side.bottom[3]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));
		} else {
		}

		//NEAR
		ptr = bNear;
		if (ptr != nullptr && (ptr->type == AIR || ptr->type == LIQUID || ptr->type == GLASS)) {
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));
		} else {
		}

		//FAR
		ptr = bFar;
		if (ptr != nullptr && (ptr->type == AIR || ptr->type == LIQUID || ptr->type == GLASS)) {
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));
		} else {
		}
	}
	return verts;
}

Chunk::Chunk() {

}

Chunk::Chunk(vec3 l, World *in):loc(l), inWorld(in) {
	if (started) {
		buffersToGen.push_back(&vert_vbo);
		buffersToGen.push_back(&tex_vbo);
		buffersToGen.push_back(&vert_vbo_water);
		buffersToGen.push_back(&tex_vbo_water);
	} else {
		glGenBuffers(1,&vert_vbo);
		glGenBuffers(1,&tex_vbo);
		glGenBuffers(1,&vert_vbo_water);
		glGenBuffers(1,&tex_vbo_water);
	}

	hash = vec3Hash(loc);

	block.resize(CHUNK_WIDTH);
	for (int x = 0; x < CHUNK_WIDTH; x++) {
		block[x].resize(CHUNK_HEIGHT);
		for (int y = 0; y < CHUNK_HEIGHT; y++) {
			block[x][y].resize(CHUNK_DEPTH);
		}
	}

	for (float y = 0; y < CHUNK_HEIGHT; y++) {
		for (float z = 0; z < CHUNK_DEPTH; z++) {
			for (float x = 0; x < CHUNK_WIDTH; x++) {
				block[x][y][z] = inWorld->generateBlock(vec3(x+loc.x,y+loc.y,z+loc.z), this);
				block[x][y][z].inChunk = this;
				block[x][y][z].worldIn = inWorld;

				block[x][y][z].edge = 0;

				if (!x)block[x][y][z].edge |= LEFT;
				if (x == CHUNK_WIDTH-1)block[x][y][z].edge |= RIGHT;

				if (!y)block[x][y][z].edge |= BOTTOM;
				if (y == CHUNK_HEIGHT-1)block[x][y][z].edge |= TOP;

				if (!z)block[x][y][z].edge |= NEAR;
				if (z == CHUNK_DEPTH-1)block[x][y][z].edge |= FAR;

				//std::cout << "Edge:" << block[x][y][z].edge << std::endl;
			}
		}
	}
}

void Chunk::updateBlocks()
{
	std::vector<std::pair<vec3,vec2>>qwer;
	for (float y = 0; y <= highest; y++) {
		for (float z = 0; z < CHUNK_DEPTH; z++) {
			for (float x = 0; x < CHUNK_WIDTH; x++) {
				block[x][y][z].inChunk = this;
				block[x][y][z].worldIn = inWorld;
				qwer = block[x][y][z].updateFaces(false);
				if (block[x][y][z].type == LIQUID) {
					for (auto &q : qwer) {
						vertex_water.push_back(q.first);
						tex_coord_water.push_back(q.second);
						elements_water++;
					}
				} else {
					for (auto &q : qwer) {
						vertex.push_back(q.first);
						tex_coord.push_back(q.second);
						elements++;
					}
				}
			}
		}
	}

	if (started) {
		chunkPtrs.push_back(this);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(vec3), &vertex[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
		glBufferData(GL_ARRAY_BUFFER, tex_coord.size() * sizeof(vec2), &tex_coord[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_water);
		glBufferData(GL_ARRAY_BUFFER, vertex_water.size() * sizeof(vec3), &vertex_water[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_water);
		glBufferData(GL_ARRAY_BUFFER, tex_coord_water.size() * sizeof(vec2), &tex_coord_water[0], GL_STATIC_DRAW);

		canRender = true;
	}


	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);*/
}

void Chunk::render()
{
	if (!canRender)return;
	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
	glVertexAttribPointer(attribute_t_index, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
	glVertexAttribPointer(attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, elements);

}

void Chunk::renderL()
{
	if (!canRender)return;
	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_water);
	glVertexAttribPointer(attribute_t_index, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_water);
	glVertexAttribPointer(attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, elements_water);
}

World::World()
{

}

float World::noise2D(float x, float y, int octaves, float persistance)
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

Block World::generateBlock(vec3 l, Chunk *ptr)
{
	Block b;

	// determine the height of the blocks
	float n = noise2D((l.x) / 256.0, (l.z) / 256.0, 3, 0.8) * 4;
	int h = n * 2;
	h += SEA;

	// if the current block is less than the height described
	// we will make it solid
	if (l.y <= h) {
		b.type = SOLID;
		if (l.y == h) {
			b.side = block_sides(vec2(0,1),vec2(0,2),vec2(0,0)); //grass
		} else {
			b.side = block_sides(vec2(0,0),vec2(0,0),vec2(0,0)); //dirt
		}
	} else {
		b.type = AIR;
	}

	// if a block is air, and it's less than the sea level
	// we make it the sea
	if (b.type == AIR && l.y <= SEA) {
		b.type = LIQUID;
		b.side = block_sides(vec2(3,2),vec2(3,2),vec2(3,2)); //water
	}

	// if we gave a chunk, then we update the highest block
	// in the chunk, this will make chunk update times faster
	if (ptr)
		if (l.y > ptr->highest)ptr->highest = l.y;

	b.loc = l;

	return b;
}

void World::createChunk(vec3 l)
{
	mtx.lock();
	chunk.emplace(vec3Hash(l),Chunk(l,this));
	mtx.unlock();
	//chunk.rehash();
}

void World::updateChunks()
{
	for (auto &c : chunk) {
		c.second.inWorld = this;
		c.second.updateBlocks();
		//thr.Enqueue([&]{c.second.updateBlocks();});
	}
}

void World::updateChunk(vec3 l)
{
	Chunk *ptr = chunkAt(l);
	if (ptr)
		ptr->updateBlocks();
}

Block::~Block()
{
	inChunk = nullptr;
	worldIn = nullptr;
}

Chunk::~Chunk()
{
	inWorld = nullptr;

	glDeleteBuffers(1, &vert_vbo);
	glDeleteBuffers(1, &vert_vbo_water);
	glDeleteBuffers(1, &tex_vbo);
	glDeleteBuffers(1, &tex_vbo_water);
}
