#include <Block.h>

extern World world;

std::mutex mtx;
ThreadPool thr(50);

Block::Block(){

}

Block::Block(vec3 l){
	loc = l;
	hash = vec3Hash(l);

	verts.reserve(24);
	colors.reserve(24);
}

Block* World::blockAt(vec3 l){
	//std::cout << l.x << "," << l.y << "," << l.z << std::endl;
/*
	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	//std::cout << buf.x << "," << buf.y << "," << buf.z << std::endl;

	std::unordered_map<uint, Chunk>::iterator chu = chunk.find(vec3Hash(buf));

	if (chu == chunk.end() )
		return nullptr;

	Chunk c = chu->second;
	std::unordered_map<uint, Block>::iterator blo = c.block.find(vec3Hash(l));

	if (blo == c.block.end() )
		return nullptr;
		*/
	Block *b = new Block(l);
	return b;
}

bool World::blockIsAir(vec3 l){
	//mtx.lock();
	//std::cout << l.x << "," << l.y << "," << l.z << std::endl;

	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	//std::cout << buf.x << "," << buf.y << "," << buf.z << std::endl;

	std::unordered_map<uint, Chunk>::iterator chu = chunk.find(vec3Hash(buf));

	if(chu == chunk.end())
		return true;

	Chunk c = chu->second;
	//std::unordered_map<uint, Block>::iterator blo = c.block.find(vec3Hash(l));

	//if (blo == c.block.end() )
		//return true;

	if(c.block[l.x-buf.x][l.y-buf.y][l.z-buf.z].type==AIR)
		return true;
	//mtx.unlock();
	return false;
}

void Block::update(){
	//RIGHT
		if(world.blockIsAir({loc.x+1,loc.y,loc.z})){
			mtx.lock();
			//std::cout << "Air to right" << std::endl;
			verts.push_back({loc.x+1,loc.y,loc.z});
			verts.push_back({loc.x+1,loc.y,loc.z+1});
			verts.push_back({loc.x+1,loc.y+1,loc.z+1});
			verts.push_back({loc.x+1,loc.y+1,loc.z});

			colors.push_back(color);
			colors.push_back(color);
			colors.push_back(color);
			colors.push_back(color);
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
			colors.push_back(color);
			colors.push_back(color);
			colors.push_back(color);
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
			colors.push_back(color);
			colors.push_back(color);
			colors.push_back(color);
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
			colors.push_back(color);
			colors.push_back(color);
			colors.push_back(color);
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
			colors.push_back(color);
			colors.push_back(color);
			colors.push_back(color);
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
			colors.push_back(color);
			colors.push_back(color);
			colors.push_back(color);
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
				std::cout << "Setting loc" << std::endl;
				std::cout << x << "," << y << "," << z << std::endl;
				block[x][y][z].loc = {x+loc.x,y+loc.y,z+loc.z};
				std::cout << "Setting type" << std::endl;
				block[x][y][z].type = SOLID;

				if(y == CHUNK_HEIGHT-1){
					block[x][y][z].color = {0.0f,1.0f,0.0f};
				}else{
					block[x][y][z].color = {1.0f,0.25f,0.25f};
				}
			}

		}
	}
	std::cout << "End of chunk" << std::endl;
}

void Chunk::updateBlocks(){
	for(float y = 0; y < CHUNK_HEIGHT; y++){
		for(float z = 0; z < CHUNK_DEPTH; z++){
			for(float x = 0; x < CHUNK_WIDTH; x++){
				block[x][y][z].update();
			}

		}
	}

	/*thr.Enqueue([&]{
	for(float y = loc.y; y < loc.y + (CHUNK_HEIGHT/4); y++){
		for(float z = loc.z; z < loc.z + CHUNK_DEPTH; z++){
			for(float x = loc.x; x < loc.x + CHUNK_WIDTH; x++){
				block.at(vec3Hash({x,y,z})).update();
			}

		}
	}

	for(float y = loc.y + (CHUNK_HEIGHT/4); y < loc.y + (CHUNK_HEIGHT/4)*2; y++){
		for(float z = loc.z; z < loc.z + CHUNK_DEPTH; z++){
			for(float x = loc.x; x < loc.x + CHUNK_WIDTH; x++){
				block.at(vec3Hash({x,y,z})).update();
			}

		}
	}

	for(float y = loc.y + (CHUNK_HEIGHT/4)*2; y < loc.y + (CHUNK_HEIGHT/4)*3; y++){
		for(float z = loc.z; z < loc.z + CHUNK_DEPTH; z++){
			for(float x = loc.x; x < loc.x + CHUNK_WIDTH; x++){
				block.at(vec3Hash({x,y,z})).update();
			}

		}
	}

	for(float y = loc.y + (CHUNK_HEIGHT/4)*3; y < loc.y + (CHUNK_HEIGHT/4)*4; y++){
		for(float z = loc.z; z < loc.z + CHUNK_DEPTH; z++){
			for(float x = loc.x; x < loc.x + CHUNK_WIDTH; x++){
				block.at(vec3Hash({x,y,z})).update();
			}

		}
	}
});*/
}

World::World(){

}

void World::createChunk(vec3 l){
	chunk.emplace(vec3Hash(l),Chunk(l));
}

void World::updateChunks(){
	uint a = 0;
	std::cout << "Chunk size: " << chunk.size() << std::endl;
	for(auto &c : chunk){
		//thr.Enqueue([&]{c.second.updateBlocks();});
		c.second.updateBlocks();
		std::cout << "Chunk: " << a++ << std::endl;
	}
}
