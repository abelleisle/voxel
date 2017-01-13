/** @file common.h
 * @brief Common items needed by most other files.
 *
 * This file contains headers, variables and functions that are needed in
 * most other files included in this project.
 */

#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <thread>
#include <mutex>
#include <future>
#include <algorithm>
#include <unordered_map>
#include <memory>

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <shader_utils.hpp>

const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 32;
const int CHUNK_DEPTH = 16;

const int SUPER_WIDTH = 32;
const int SUPER_HEIGHT = 2;
const int SUPER_DEPTH = 32;

const int CHUNK_RAD = 12;

class vec2{
public:
	float x;
	float y;

	vec2(float _x, float _y) {
		x = _x;
		y = _y;
	}
	vec2() {x=y=0;}
};

class vec3{
public:
	float x;
	float y;
	float z;

	vec3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}
	vec3() {x=y=z=0;}
	vec3 operator+(vec3 &o) {
		return vec3(this->x + o.x, this->y + o.y, this->z + o.z);
	}
};

class blockData{
public:
	float x;
	float y;
	float z;
	float w;

	blockData(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	blockData() {x=y=z=w=0;}
};

typedef struct{
	float r;
	float g;
	float b;
}Color;

long long vec3Hash(vec3 l);

vec3 vec3FromHash(long long hash);

template<typename N, size_t s>
size_t arrAmt(N (&)[s]) {return s;}

#endif // COMMON_H
