#include <common.hpp>

long long vec3Hash(vec3 l) {
	return ((uint64_t)l.x << 32) | ((uint32_t)l.y << 16) | ((uint16_t)l.z);
}

vec3 vec3FromHash(long long hash) {
	vec3 lo;
	lo.z = hash & 0xff;
	lo.y = (hash >>= 16) & 0xff;
	lo.x = (hash >>= 16) & 0xff;
	return lo;
}

float blockIndex(uint id, uint side) {
	return id +(side/10.0f);
}
