#include <Block.h>

extern World world;

Block::Block(){

}

Block::Block(vec3 l){
	loc = l;
	hash = vec3Hash(l);

	id = 1;
}

Block* World::blockAt(vec3 l){
	std::cout << l.x << "," << l.y << "," << l.z << std::endl;

	vec3 buf;
	buf.x = floor(l.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	buf.y = floor(l.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	buf.z = floor(l.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	std::cout << buf.x << "," << buf.y << "," << buf.z << std::endl;

	std::unordered_map<uint, Chunk>::const_iterator chu = chunk.find(vec3Hash(buf));

	if (chu == chunk.end() )
		return nullptr;

	Chunk c = chu->second;
	std::unordered_map<uint, Block>::const_iterator blo = c.block.find(vec3Hash(l));

	if (blo == c.block.end() )
		return nullptr;

	std::cout << "Crashb" << std::endl;
	Block *b = (Block*)&blo->second;
	std::cout << "Crasha" << std::endl;
	std::cout << b->loc.x << std::endl;
	return b;
}

void Block::update(){
	std::cout << "Crash" << std::endl;

	Block 	*blockR = world.blockAt({loc.x+1,loc.y,loc.z});
	std::cout << "f" << std::endl;
	Block 	*blockL = world.blockAt({loc.x-1,loc.y,loc.z}),
			*blockT = world.blockAt({loc.x,loc.y+1,loc.z}),
			*blockB = world.blockAt({loc.x,loc.y-1,loc.z}),
			*blockN = world.blockAt({loc.x,loc.y,loc.z+1}),
			*blockF = world.blockAt({loc.x,loc.y,loc.z-1});

	std::cout << "Crash2" << std::endl;
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
}

Chunk::Chunk(){

}

Chunk::Chunk(vec3 l):loc(l){
	hash = vec3Hash(l);
	for(float y = loc.y; y < loc.y + CHUNK_HEIGHT; y++){
		for(float z = loc.z; z < loc.z + CHUNK_DEPTH; z++){
			for(float x = loc.x; x < loc.x + CHUNK_WIDTH; x++){
				block.emplace(vec3Hash({x,y,z}),Block({x,y,z}));
			}

		}
	}
}

void Chunk::updateBlocks(){
	
}

World::World(){
	
}

void World::createChunk(vec3 l){
	chunk.emplace(vec3Hash(l),Chunk(l));
}

void World::updateChunks(){
	
}