#include <Block.h>

Block* World::blockAt(vec3 l){
	vec3 chunkl = {0,0,0};
	vec3 blockl = {0,0,0};

	chunkl.x = (int)l.x / CHUNK_WIDTH;
	chunkl.y = (int)l.y / CHUNK_HEIGHT;
	chunkl.z = (int)l.z / CHUNK_DEPTH;

	blockl.x = (int)l.x % CHUNK_WIDTH;
	blockl.y = (int)l.y % CHUNK_HEIGHT;
	blockl.z = (int)l.z % CHUNK_DEPTH;

	return &chunk[chunkl.y][chunkl.x][chunkl.z].block[blockl.y][blockl.x][blockl.z];

}

void Block::update(){

}

Chunk::Chunk(vec3 l):loc(l){
	/*block = new Block**[CHUNK_HEIGHT];
	for (int i = 0; i < CHUNK_HEIGHT; i++) {
		block[i] = new Block*[CHUNK_WIDTH];

	for (int j = 0; j < CHUNK_WIDTH; j++)
			block[i][j] = new Block[CHUNK_DEPTH];
	}	*/

	for(uint h = 0; h < CHUNK_HEIGHT; h++){	
		for(uint w = 0; w < CHUNK_WIDTH; w++){
			for(uint d = 0; d < CHUNK_DEPTH; d++){
				block[h][w][d].loc = {loc.x+(float)w,loc.y+(float)h,loc.z+(float)d};
				block[h][w][d].id = 1;
				block[h][w][d].update();
			}
		}
	}
}