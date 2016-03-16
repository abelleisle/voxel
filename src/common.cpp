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