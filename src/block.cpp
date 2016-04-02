#include <block.hpp>

extern GLint attribute_coord;
extern GLint attribute_t_index;

extern World world;

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

	uint hash = vec3Hash(buf);
	Chunk *chunkPtr = nullptr;

	for(auto &c : chunk){
		if(c.second.hash == hash){
			chunkPtr = &c.second;
		}
	}
	if(chunkPtr==nullptr)
		return nullptr;

	return &chunkPtr->block[l.x-buf.x][l.y-buf.y][l.z-buf.z];
}

bool World::blockIsAir(vec3 l){
	//std::cout << "testing for air" << std::endl;
	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	//std::cout << buf.x << "," << buf.y << "," << buf.z << std::endl;

	uint hash = vec3Hash(buf);
	Chunk *chunkPtr = nullptr;

	for(auto &c : chunk){
		if(c.second.hash == hash){
			chunkPtr = &c.second;
		}
	}
	if(chunkPtr==nullptr)
		return true;


	if(chunkPtr->block[l.x-buf.x][l.y-buf.y][l.z-buf.z].type==AIR)
		return true;

	return false;
}

std::vector<std::pair<vec3,vec2>> Block::updateFaces(){
	std::vector<std::pair<vec3,vec2>> verts;
	if(this->type == AIR)return std::vector<std::pair<vec3,vec2>>();
	//RIGHT
	if(inChunk->inWorld->blockIsAir({loc.x+1,loc.y,loc.z})){
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z+1), side.sides[1]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[3]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,  loc.z), side.sides[0]));

	}else{
	}

	//LEFT
	if(inChunk->inWorld->blockIsAir({loc.x-1,loc.y,loc.z})){
		verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));
		verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z+1), side.sides[1]));
		verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));

		verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[2]));
		verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
		verts.push_back(std::make_pair(vec3(loc.x,loc.y,  loc.z), side.sides[0]));

	}else{
	}

	//TOP
	if(inChunk->inWorld->blockIsAir({loc.x,loc.y+1,loc.z})){
		verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.top[1]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));

		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.top[2]));
		verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z+1), side.top[3]));
		verts.push_back(std::make_pair(vec3(loc.x,  loc.y+1,loc.z), side.top[0]));
	}else{
	}

	//BOTTOM
	if(inChunk->inWorld->blockIsAir({loc.x,loc.y-1,loc.z})){
		verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.bottom[1]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));

		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.bottom[2]));
		verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z+1), side.bottom[3]));
		verts.push_back(std::make_pair(vec3(loc.x,  loc.y,loc.z), side.bottom[0]));

	}else{
	}

	//NEAR
	if(inChunk->inWorld->blockIsAir({loc.x,loc.y,loc.z-1})){

		verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z), side.sides[1]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));

		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z), side.sides[2]));
		verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z), side.sides[3]));
		verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z), side.sides[0]));

	}else{
	}

	//FAR
	if(inChunk->inWorld->blockIsAir({loc.x,loc.y,loc.z+1})){

		verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y,loc.z+1), side.sides[1]));
		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));

		verts.push_back(std::make_pair(vec3(loc.x+1,loc.y+1,loc.z+1), side.sides[2]));
		verts.push_back(std::make_pair(vec3(loc.x,loc.y+1,loc.z+1), side.sides[3]));
		verts.push_back(std::make_pair(vec3(loc.x, loc.y, loc.z+1), side.sides[0]));

	}else{
	}
	return verts;
}

Chunk::Chunk(){

}

Chunk::Chunk(vec3 l, World *in):loc(l), inWorld(in){
	glGenBuffers(1,&vert_vbo);
	glGenBuffers(1,&tex_vbo);
	hash = vec3Hash(loc);
	block.resize(CHUNK_WIDTH);
	for(int x = 0; x < CHUNK_WIDTH; x++){
		block[x].resize(CHUNK_HEIGHT);
		for(int y = 0; y < CHUNK_HEIGHT; y++){
			block[x][y].resize(CHUNK_DEPTH);
		}
	}
	for(float y = 0; y < CHUNK_HEIGHT; y++){
		for(float z = 0; z < CHUNK_DEPTH; z++){
			for(float x = 0; x < CHUNK_WIDTH; x++){

				block[x][y][z] = this->inWorld->generateBlock(vec3(x+loc.x,y+loc.y,z+loc.z));
				block[x][y][z].inChunk = this;
			}

		}
	}
	std::cout << "End of chunk" << std::endl;
}

void Chunk::updateBlocks(){
	std::vector<std::pair<vec3,vec2>>qwer;
	for(float y = 0; y < CHUNK_HEIGHT; y++){
		for(float z = 0; z < CHUNK_DEPTH; z++){
			for(float x = 0; x < CHUNK_WIDTH; x++){
				qwer = block[x][y][z].updateFaces();
				for(auto &q : qwer){
					vertex.push_back(q.first);
					tex_coord.push_back(q.second);
					elements++;
				}
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vert_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(vec3), &vertex[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
	glBufferData(GL_ARRAY_BUFFER, tex_coord.size() * sizeof(vec2), &tex_coord[0], GL_STATIC_DRAW);


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

World::World(){

}

Block World::generateBlock(vec3 l){
	Block b;
	if(l.y <= floor(4*sin(.25*(l.x + l.z)))+12){
		b.type = SOLID;
	}else{
		b.type = AIR;
	}
	if(l.y >= floor(4*sin(.25*(l.x + l.z)))+12){
		b.side = block_sides(vec2(0,1),vec2(0,2),vec2(0,0));
	}else{
		b.side = block_sides(vec2(0,0),vec2(0,0),vec2(0,0));
	}

	//b.color = {0.0f,1.0f,0.0f};
	b.loc = l;

	return b;
}

void World::createChunk(vec3 l){
	chunk.emplace(vec3Hash(l),Chunk(l,this));
	//chunk.rehash();
}

void World::updateChunks(){
	uint a = 0;
	for(auto &c : chunk){
		c.second.updateBlocks();
		//thr.Enqueue([&]{c.second.updateBlocks();});
	}
}
