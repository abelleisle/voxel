#include <Block.h>

extern World world;

Block::Block(){

}

Block* World::blockAt(vec3 l){
	vec3 chunkl = {0,0,0};
	vec3 blockl = {0,0,0};

	chunkl.x = (int)l.x / (int)CHUNK_WIDTH;
	chunkl.y = (int)l.y / (int)CHUNK_HEIGHT;
	chunkl.z = (int)l.z / (int)CHUNK_DEPTH;

	blockl.x = (int)l.x % (int)CHUNK_WIDTH;
	blockl.y = (int)l.y % (int)CHUNK_HEIGHT;
	blockl.z = (int)l.z % (int)CHUNK_DEPTH;

	//std::cout << "Chunk at: " << chunkl.x << "," << chunkl.y << "," << chunkl.z << std::endl;
	//std::cout << "Block at: " << blockl.x << "," << blockl.y << "," << blockl.z << std::endl;

	if(blockl.y < 0 || blockl.y > (CHUNK_HEIGHT-1)){
		return NULL;
	}
	if(blockl.x < 0 || blockl.x > (CHUNK_WIDTH-1)){
		return NULL;
	}
	if(blockl.z < 0 || blockl.z > (CHUNK_DEPTH-1)){
		return NULL;
	}

	if(chunkl.y >= chunk.size()){
		return NULL;
	}
	if(chunkl.x >= chunk[0].size()){
		return NULL;
	}
	if(chunkl.z >= chunk[0][0].size()){
		return NULL;
	}

	//std::cout << chunk[(int)chunkl.y][(int)chunkl.x][(int)chunkl.z].block[(int)blockl.y][(int)blockl.x][(int)blockl.z].id << std::endl;
	return &chunk[(int)chunkl.y][(int)chunkl.x][(int)chunkl.z].block[(int)blockl.y][(int)blockl.x][(int)blockl.z];;

}

void Block::update(){
	Block 	*blockR = world.blockAt({loc.x+1,loc.y,loc.z}), 
			*blockL = world.blockAt({loc.x-1,loc.y,loc.z}),
			*blockT = world.blockAt({loc.x,loc.y+1,loc.z}),
			*blockB = world.blockAt({loc.x,loc.y-1,loc.z}),
			*blockN = world.blockAt({loc.x,loc.y,loc.z+1}),
			*blockF = world.blockAt({loc.x,loc.y,loc.z-1});
	//right
	if(blockR){
		if(blockR->id != 0){
			std::cout << "Something on the right" << std::endl;
		}
	}
	//top
	if(blockT){
		if(blockT->id != 0){
			std::cout << "Something on the top" << std::endl;
		}
	}
	//near
	if(blockN){

		if(blockN->id != 0){
			std::cout << "Something on the near" << std::endl;
			std::cout << inChunk->vertOrder.size() << std::endl;
			inChunk->verts.push_back(loc.x);
			inChunk->verts.push_back(loc.y);
			inChunk->verts.push_back(loc.z);
			inChunk->vertOrder.push_back(0+inChunk->vertOrder.size());

			inChunk->verts.push_back(loc.x+1);
			inChunk->verts.push_back(loc.y);
			inChunk->verts.push_back(loc.z);
			inChunk->vertOrder.push_back(1+inChunk->vertOrder.size());

			inChunk->verts.push_back(loc.x+1);
			inChunk->verts.push_back(loc.y+1);
			inChunk->verts.push_back(loc.z);
			inChunk->vertOrder.push_back(2+inChunk->vertOrder.size());

			inChunk->verts.push_back(loc.x);
			inChunk->verts.push_back(loc.y+1);
			inChunk->verts.push_back(loc.z);
			inChunk->vertOrder.push_back(3+inChunk->vertOrder.size());
		}
	}
	//left
	if(blockL){
		if(blockL->id != 0){
			std::cout << "Something on the left" << std::endl;
		}
	}
	//bottom
	if(blockB){
		if(blockB->id != 0){
			std::cout << "Somthing on the bottom" << std::endl;
		}
	}
	//far
	if(blockF){
		if(blockF->id != 0){
			std::cout << "Something on the far" << std::endl;
		}
	}

	/*world->blockAt(loc.x+1,loc.y+1,loc.z);
		world->blockAt(loc.x+1,loc.y+1,loc.z+1);
		world->blockAt(loc.x+1,loc.y+1,loc.z-1);
	world->blockAt(loc.x+1,loc.y-1,loc.z);
		world->blockAt(loc.x+1,loc.y-1,loc.z+1);
		world->blockAt(loc.x+1,loc.y-1,loc.z-1);

	world->blockAt(loc.x-1,loc.y+1,loc.z);
		world->blockAt(loc.x-1,loc.y+1,loc.z+1);
		world->blockAt(loc.x-1,loc.y+1,loc.z-1);
	world->blockAt(loc.x-1,loc.y-1,loc.z);
		world->blockAt(loc.x-1,loc.y-1,loc.z+1);
		world->blockAt(loc.x-1,loc.y-1,loc.z-1);

	world->blockAt(loc.x,loc.y+1,loc.z+1);
	world->blockAt(loc.x,loc.y-1,loc.z+1);

	world->blockAt(loc.x,loc.y+1,loc.z-1);
	world->blockAt(loc.x,loc.y-1,loc.z-1);*/
}

Chunk::Chunk(){

}

Chunk::Chunk(vec3 l):loc(l){
	//verts.resize(0);
	//vertOrderList.resize(0);
	//uint num = 0;
	for(uint h = 0; h < (int)CHUNK_HEIGHT; h++){	
		for(uint w = 0; w < (int)CHUNK_WIDTH; w++){
			for(uint d = 0; d < (int)CHUNK_DEPTH; d++){
				block[h][w][d].loc = {loc.x+(float)w,loc.y+(float)h,loc.z+(float)d};
				if(h == CHUNK_HEIGHT)
					block[h][w][d].color = {0,1,0};
				else
					block[h][w][d].color = {1,.5,.25};
				block[h][w][d].id = 1;
				block[h][w][d].inChunk = this;
			}
		}
	}
	/*for(uint h = 0; h <= (int)CHUNK_HEIGHT; h++){	
		for(uint w = 0; w <= (int)CHUNK_WIDTH; w++){
			for(uint d = 0; d <= (int)CHUNK_DEPTH; d++){
				verts.push_back(loc.x+(float)w);
				verts.push_back(loc.y+(float)h);
				verts.push_back(loc.z+(float)d);
				vertOrderList.push_back(num);
				num++;
			}
		}
	}*/
}

void Chunk::updateBlocks(){
	for(auto &y : block){
		for(auto &x : y){
			for(auto &z : x){
				z.update();
			}
		}
	}
}

World::World(){
	chunk.resize(1);
	for(int i = 0; i < 1; i++){
		chunk[i].resize(1);
		for(int j = 0; j < 1; j++){
			chunk[i][j].resize(1);
		}
	}
	std::cout << chunk.size() << std::endl;
}

void World::createChunk(vec3 l){
	chunk[0][0].push_back(Chunk(l));
}

void World::updateChunks(){
	for(uint y = 0; y < chunk.size();y++){
		for(uint x = 0; x < chunk[0].size();x++){
			for(uint z = 0; z < chunk[0][0].size();z++){
				chunk[y][x][z].updateBlocks();
			}
		}
	}
}