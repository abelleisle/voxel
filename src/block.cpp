#include <block.hpp>

//extern World world;

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
		return true;

	if(chunkPtr->block[l.x-buf.x][l.y-buf.y][l.z-buf.z].type==AIR)
		return true;

	return false;
}

std::vector<vec3> Block::updateFaces(){
	verts.clear();
	if(this->type == AIR)return verts;
	//RIGHT
		if(inChunk->inWorld->blockIsAir({loc.x+1,loc.y,loc.z})){
			mtx.lock();
			//bottom left
			verts.push_back({loc.x+1,loc.y,loc.z});
			//bottom right
			verts.push_back({loc.x+1,loc.y,loc.z+1});
			//top right
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});

			//bottom left
			verts.push_back({loc.x+1,loc.y,loc.z});
			//top right
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});
			//top left
			verts.push_back({loc.x+1,loc.y+1,loc.z});



			colors.push_back(color);

			mtx.unlock();
		}else{
		}

	//LEFT
		if(inChunk->inWorld->blockIsAir({loc.x-1,loc.y,loc.z})){
			mtx.lock();
			verts.push_back({loc.x,loc.y,loc.z});
			verts.push_back({loc.x,loc.y,loc.z+1});
			verts.push_back({loc.x,loc.y+1,loc.z+1});

			verts.push_back({loc.x,loc.y,loc.z});
			verts.push_back({loc.x,loc.y+1,loc.z+1});
			verts.push_back({loc.x,loc.y+1,loc.z});


			colors.push_back(color);

			mtx.unlock();
		}else{
		}

	//TOP
		if(inChunk->inWorld->blockIsAir({loc.x,loc.y+1,loc.z})){
			mtx.lock();
			verts.push_back({loc.x,loc.y+1,loc.z});
			verts.push_back({loc.x+1,loc.y+1,loc.z});
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});

			verts.push_back({loc.x,loc.y+1,loc.z});
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});
			verts.push_back({loc.x,loc.y+1,loc.z+1});


			colors.push_back(color);

			mtx.unlock();
		}else{
		}

	//BOTTOM
		if(inChunk->inWorld->blockIsAir({loc.x,loc.y-1,loc.z})){
			mtx.lock();
			verts.push_back({loc.x,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y,loc.z+1});

			verts.push_back({loc.x,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y,loc.z+1});
			verts.push_back({loc.x,loc.y,loc.z+1});


			colors.push_back(color);

			mtx.unlock();
		}else{
		}

	//NEAR
		if(inChunk->inWorld->blockIsAir({loc.x,loc.y,loc.z-1})){
			mtx.lock();
			verts.push_back({loc.x,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y+1,loc.z});

			verts.push_back({loc.x,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y+1,loc.z});
			verts.push_back({loc.x,loc.y+1,loc.z});

			colors.push_back(color);

			mtx.unlock();
		}else{
		}

	//FAR
		if(inChunk->inWorld->blockIsAir({loc.x,loc.y,loc.z+1})){
			mtx.lock();
			verts.push_back({loc.x,loc.y,loc.z+1});
			verts.push_back({loc.x+1,loc.y,loc.z+1});
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});

			verts.push_back({loc.x,loc.y,loc.z+1});
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});
			verts.push_back({loc.x,loc.y+1,loc.z+1});

			colors.push_back(color);

			mtx.unlock();
		}else{
		}
	return verts;
}

Chunk::Chunk(){

}

Chunk::Chunk(vec3 l):loc(l){
	std::cout << "Chunk constructor" << std::endl;
	hash = vec3Hash(l);
	block.resize(CHUNK_WIDTH);
	std::cout << "Chunk constructor resize after width" << std::endl;
	for(int x = 0; x < CHUNK_WIDTH; x++){
		block[x].resize(CHUNK_HEIGHT);
		for(int y = 0; y < CHUNK_HEIGHT; y++){
			block[x][y].resize(CHUNK_DEPTH);
		}
	}
	std::cout << "Chunk constructor resize after all" << std::endl;
	for(float y = 0; y < CHUNK_HEIGHT; y++){
		for(float z = 0; z < CHUNK_DEPTH; z++){
			for(float x = 0; x < CHUNK_WIDTH; x++){
				//block.emplace(vec3Hash({x,y,z}),Block({x,y,z}));
				/*std::cout << "Setting loc" << std::endl;
				std::cout << x << "," << y << "," << z << std::endl;
				block[x][y][z].loc = {x+loc.x,y+loc.y,z+loc.z};
				std::cout << "Setting type" << std::endl;
				block[x][y][z].type = SOLID;

				if(y == CHUNK_HEIGHT-1){
					block[x][y][z].color = {0.0f,1.0f,0.0f};
				}else{
					block[x][y][z].color = {1.0f,0.25f,0.25f};
				}*/
				block[x][y][z] = this->inWorld->generateBlock({x+loc.x,y+loc.y,z+loc.z});
			}

		}
	}
	std::cout << "End of chunk" << std::endl;
}

void Chunk::updateBlocks(){
	std::vector<vec3>qwer;
	for(float y = 0; y < CHUNK_HEIGHT; y++){
		for(float z = 0; z < CHUNK_DEPTH; z++){
			for(float x = 0; x < CHUNK_WIDTH; x++){
				qwer = block[x][y][z].updateFaces();
				for(auto &q : qwer){
					vertex.push_back(q);
				}
				elements++;
			}
		}
	}
	std::cout << "sdf" << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, elements * sizeof vertex, &vertex[0], GL_STATIC_DRAW);
	std::cout << "asdas" << std::endl;
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
		b.color = {0.0f,1.0f,0.0f};
	}else{
		b.color = {1.0f,.25f,.25f};
	}

	//b.color = {0.0f,1.0f,0.0f};
	b.loc = l;

	return b;
}

void World::createChunk(vec3 l){
	Chunk temp(l);
	temp.inWorld = this;
	chunk.emplace(vec3Hash(l),temp);
	//chunk.rehash();
}

void World::updateChunks(){
	uint a = 0;
	std::cout << "Chunk size: " << chunk.size() << std::endl;
	for(auto &c : chunk){
		c.second.updateBlocks();
		//thr.Enqueue([&]{c.second.updateBlocks();});
		std::cout << "Chunk: " << a++ << std::endl;
	}
}
