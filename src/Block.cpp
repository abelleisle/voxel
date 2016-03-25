#include <Block.h>

extern World world;

std::mutex mtx;
std::mutex blockMtx;
ThreadPool thr(50);

Block::Block(){
}

Block::Block(vec3 l){
	loc = l;
	hash = vec3Hash(l);

	verts.reserve(24);
	colors.reserve(24);

	texture = Texture::loadTexture("meme.png");
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

void Block::updateFaces(){
	if(this->type == AIR)return;
	//RIGHT
		if(world.blockIsAir({loc.x+1,loc.y,loc.z})){
			mtx.lock();
			//std::cout << "Air to right" << std::endl;
			verts.push_back({loc.x+1,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y,loc.z+1});
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});
			verts.push_back({loc.x+1,loc.y+1,loc.z});

			colors.push_back(color);

			normals.push_back(RIGHT);
			mtx.unlock();
		}else{
			//std::cout << "No air" << std::endl;
		}

	//LEFT
		if(world.blockIsAir({loc.x-1,loc.y,loc.z})){
			mtx.lock();
			//std::cout << "Air to right" << std::endl;
			verts.push_back({loc.x,loc.y,loc.z});
			verts.push_back({loc.x,loc.y,loc.z+1});
			verts.push_back({loc.x,loc.y+1,loc.z+1});
			verts.push_back({loc.x,loc.y+1,loc.z});

			colors.push_back(color);

			normals.push_back(LEFT);
			mtx.unlock();
		}else{
			//std::cout << "No air" << std::endl;
		}

	//TOP
		if(world.blockIsAir({loc.x,loc.y+1,loc.z})){
			mtx.lock();
			verts.push_back({loc.x,loc.y+1,loc.z});
			verts.push_back({loc.x+1,loc.y+1,loc.z});
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});
			verts.push_back({loc.x,loc.y+1,loc.z+1});

			colors.push_back(color);

			normals.push_back(TOP);
			mtx.unlock();
		}else{
			//std::cout << "No air" << std::endl;
		}

	//BOTTOM
		if(world.blockIsAir({loc.x,loc.y-1,loc.z})){
			mtx.lock();
			verts.push_back({loc.x,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y,loc.z+1});
			verts.push_back({loc.x,loc.y,loc.z+1});

			colors.push_back(color);

			normals.push_back(BOTTOM);
			mtx.unlock();
		}else{
			//std::cout << "No air" << std::endl;
		}

	//NEAR
		if(world.blockIsAir({loc.x,loc.y,loc.z-1})){
			mtx.lock();
			verts.push_back({loc.x,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y+1,loc.z});
			verts.push_back({loc.x,loc.y+1,loc.z});

			colors.push_back(color);

			normals.push_back(NEAR);
			mtx.unlock();
		}else{
			//std::cout << "No air" << std::endl;
		}

	//FAR
		if(world.blockIsAir({loc.x,loc.y,loc.z+1})){
			mtx.lock();
			verts.push_back({loc.x,loc.y,loc.z+1});
			verts.push_back({loc.x+1,loc.y,loc.z+1});
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});
			verts.push_back({loc.x,loc.y+1,loc.z+1});

			colors.push_back(color);

			normals.push_back(FAR);
			mtx.unlock();
		}else{
			//std::cout << "No air" << std::endl;
		}
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
	for(float y = 0; y < CHUNK_HEIGHT; y++){
		for(float z = 0; z < CHUNK_DEPTH; z++){
			for(float x = 0; x < CHUNK_WIDTH; x++){
				block[x][y][z].updateFaces();
			}

		}
	}
}

World::World(){

}

Block World::generateBlock(vec3 l){
	Block b;
	b.texture = Texture::loadTexture("bgWoodTile.png");
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
	//chunk.emplace(vec3Hash(l),Chunk(l));
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
