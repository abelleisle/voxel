#include <block.hpp>

extern GLint attribute_coord;
extern GLint attribute_t_index;

extern World world;

extern std::vector<GLuint*>buffersToGen;
extern bool started;


std::mutex mtx;
std::mutex blockMtx;
ThreadPool thr(50);

Block::Block(){

}

Block::Block(vec3 l){
	loc = l;
	//hash = vec3Hash(l);

	//verts.reserve(24);
	//colors.reserve(24);
}

Block* World::blockAt(vec3 l){
	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	unsigned long long hash = vec3Hash(buf);
	Chunk *chunkPtr = nullptr;

	try{
		chunkPtr = &chunk.at(hash);
	}catch(const std::out_of_range& oor){
		return nullptr;
	}

	return &chunkPtr->block[l.x-buf.x][l.y-buf.y][l.z-buf.z];
}

Chunk* World::chunkAt(vec3 l){
	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	unsigned long long hash = vec3Hash(buf);
	Chunk *chunkPtr = nullptr;

	try{
		chunkPtr = &chunk.at(hash);
	}catch(const std::out_of_range& oor){
		return nullptr;
	}

	return chunkPtr;
}

bool World::blockIsAir(vec3 l){
	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	unsigned long long hash = vec3Hash(buf);
	Chunk *chunkPtr = nullptr;

	/*for(auto &c : chunk){
		if(c.second.hash == hash){
			chunkPtr = &c.second;
		}
	}
	if(chunkPtr == nullptr)
		return true;*/
	try{
		chunkPtr = &chunk.at(hash);
	}catch(const std::out_of_range& oor){
		return true;
	}
	block_t ty = chunkPtr->block[l.x-buf.x][l.y-buf.y][l.z-buf.z].type;

	if(ty == AIR || ty == LIQUID || ty == GLASS)
		return true;

	return false;
}

std::vector<std::pair<vec3,vec2>> Block::updateFaces(){
	std::vector<std::pair<vec3,vec2>> verts;
	if(this->type == AIR)return std::vector<std::pair<vec3,vec2>>();
	if(this->type == LIQUID || this->type == GLASS){
		Block* ptr;
		//RIGHT
		ptr = worldIn->blockAt({loc.x+1,loc.y,loc.z});
		if(ptr != nullptr && ptr->type == AIR){
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));
		}else{
		}

		//LEFT
		ptr = worldIn->blockAt({loc.x-1,loc.y,loc.z});
		if(ptr != nullptr && ptr->type == AIR){
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));
		}else{
		}

		//TOP
		ptr = worldIn->blockAt({loc.x,loc.y+1,loc.z});
		if(ptr != nullptr && ptr->type == AIR){
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.top[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z+1), side.top[3]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
		}else{
		}

		//BOTTOM
		ptr = worldIn->blockAt({loc.x,loc.y-1,loc.z});
		if(ptr != nullptr && ptr->type == AIR){
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.bottom[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z+1), side.bottom[3]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));
		}else{
		}

		//NEAR
		ptr = worldIn->blockAt({loc.x,loc.y,loc.z-1});
		if(ptr != nullptr && ptr->type == AIR){
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));
		}else{
		}

		//FAR
		ptr = worldIn->blockAt({loc.x,loc.y,loc.z+1});
		if(ptr != nullptr && ptr->type == AIR){
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));
		}else{
		}
	}else{
		//RIGHT
		if(worldIn->blockIsAir({loc.x+1,loc.y,loc.z})){
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));
		}else{
		}

		//LEFT
		if(worldIn->blockIsAir({loc.x-1,loc.y,loc.z})){
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));
		}else{
		}

		//TOP
		if(worldIn->blockIsAir({loc.x,loc.y+1,loc.z})){
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.top[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z+1), side.top[3]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
		}else{
		}

		//BOTTOM
		if(worldIn->blockIsAir({loc.x,loc.y-1,loc.z})){
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.bottom[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z+1), side.bottom[3]));
			verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));
		}else{
		}

		//NEAR
		if(worldIn->blockIsAir({loc.x,loc.y,loc.z-1})){
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));
		}else{
		}

		//FAR
		if(worldIn->blockIsAir({loc.x,loc.y,loc.z+1})){
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.sides[1]));
			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

			verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
			verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[3]));
			verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));
		}else{
		}
	}
	return verts;
}

Chunk::Chunk(){

}

Chunk::Chunk(vec3 l, World *in):loc(l), inWorld(in){
	std::cout << "Genning buffers" << std::endl;
	if(started){
		std::cout << "started" << std::endl;
		buffersToGen.push_back(&vert_vbo);
		buffersToGen.push_back(&tex_vbo);
		buffersToGen.push_back(&vert_vbo_water);
		buffersToGen.push_back(&tex_vbo_water);
	}else{
		glGenBuffers(1,&vert_vbo);
		glGenBuffers(1,&tex_vbo);
		glGenBuffers(1,&vert_vbo_water);
		glGenBuffers(1,&tex_vbo_water);
	}

	std::cout << "Setting hash" << std::endl;

	hash = vec3Hash(loc);

	std::cout << "Resizing array" << std::endl;

	block.resize(CHUNK_WIDTH);
	for(int x = 0; x < CHUNK_WIDTH; x++){
		block[x].resize(CHUNK_HEIGHT);
		for(int y = 0; y < CHUNK_HEIGHT; y++){
			block[x][y].resize(CHUNK_DEPTH);
		}
	}

	std::cout << "Setting blocks" << std::endl;

	for(float y = 0; y < CHUNK_HEIGHT; y++){
		for(float z = 0; z < CHUNK_DEPTH; z++){
			for(float x = 0; x < CHUNK_WIDTH; x++){
				block[x][y][z] = inWorld->generateBlock(vec3(x+loc.x,y+loc.y,z+loc.z));
				block[x][y][z].worldIn = inWorld;
			}

		}
	}

	std::cout << "Done setting" << std::endl;
}

void Chunk::updateBlocks(){
	std::vector<std::pair<vec3,vec2>>qwer;
	for(float y = 0; y < CHUNK_HEIGHT; y++){
		for(float z = 0; z < CHUNK_DEPTH; z++){
			for(float x = 0; x < CHUNK_WIDTH; x++){
				qwer = block[x][y][z].updateFaces();
				if(block[x][y][z].type == LIQUID){
					for(auto &q : qwer){
						vertex_water.push_back(q.first);
						tex_coord_water.push_back(q.second);
						elements_water++;
					}
				}else{
					for(auto &q : qwer){
						vertex.push_back(q.first);
						tex_coord.push_back(q.second);
						elements++;
					}
				}
			}
		}
	}

	if(!started){
		glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(vec3), &vertex[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
		glBufferData(GL_ARRAY_BUFFER, tex_coord.size() * sizeof(vec2), &tex_coord[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_water);
		glBufferData(GL_ARRAY_BUFFER, vertex_water.size() * sizeof(vec3), &vertex_water[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_water);
		glBufferData(GL_ARRAY_BUFFER, tex_coord_water.size() * sizeof(vec2), &tex_coord_water[0], GL_STATIC_DRAW);
	}


	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);*/
}

void Chunk::render(){
	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
	glVertexAttribPointer(attribute_t_index, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
	glVertexAttribPointer(attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, elements);

}

void Chunk::renderL(){
	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_water);
	glVertexAttribPointer(attribute_t_index, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_water);
	glVertexAttribPointer(attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, elements_water);
}

World::World(){

}

Block World::generateBlock(vec3 l){
	Block b;
	if(l.y <= 10+floor(5*(sin(l.x/10)*cos(l.z/10)))){
		b.type = SOLID;
	}else{
		b.type = AIR;
	}
	if(l.y >= 10+floor(5*(sin(l.x/10)*cos(l.z/10)))){
		b.side = block_sides(vec2(0,1),vec2(0,2),vec2(0,0));
	}else{
		b.side = block_sides(vec2(0,0),vec2(0,0),vec2(0,0));
	}
	if(b.type == AIR && l.y <= 8){
		b.type = LIQUID;
		b.side = block_sides(vec2(3,2),vec2(3,2),vec2(3,2));
	}
	/*if(b.type == AIR && l.y > 8 && l.y <= 9){
		b.type = GLASS;
		b.side = block_sides(vec2(4,2),vec2(4,2),vec2(4,2));
	}*/

	//b.color = {0.0f,1.0f,0.0f};
	b.loc = l;

	return b;
}

void World::createChunk(vec3 l){
	mtx.lock();
	std::cout << "Emplacing new chunk" << std::endl;
	chunk.emplace(vec3Hash(l),Chunk(l,this));
	mtx.unlock();
	//chunk.rehash();
}

void World::updateChunks(){
	for(auto &c : chunk){
		c.second.updateBlocks();
		//thr.Enqueue([&]{c.second.updateBlocks();});
	}
}

void World::updateChunk(vec3 l){
	chunkAt(l)->updateBlocks();
}
