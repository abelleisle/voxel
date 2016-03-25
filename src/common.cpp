#include <common.h>

uint vec3Hash(vec3 l){
	return ((uint8_t)l.x << 16) | ((uint8_t)l.y << 8) | ((uint8_t)l.z);
}

vec3 vec3FromHash(uint hash){
	vec3 lo;
	lo.z = hash & 0xff;
	lo.y = (hash >>= 8) & 0xff;
	lo.x = (hash >>= 8) & 0xff;
	return lo;
}

const char *readFile(const char *path){
	std::ifstream in (path,std::ios::in);
	unsigned int size;
	GLchar *buf;

	if(!in.is_open()){
		std::cout<<"Error reading file "<<path<<"!"<<std::endl;
		abort();
	}

	in.seekg(0,in.end);
	buf = new GLchar[(size = in.tellg()) + 1];
	in.seekg(0,in.beg);
	in.read(buf,size);
	buf[size] = '\0';

	in.close();
	return buf;
}
