/** @file common.h
 * @brief Common items needed by most other files.
 * 
 * This file contains headers, variables and functions that are needed in
 * most other files included in this project.
 */

#ifndef COMMON_H
#define COMMON_H

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

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

const int CHUNK_WIDTH = 8;
const int CHUNK_HEIGHT = 8;
const int CHUNK_DEPTH = 8;

typedef struct{
	float x;
	float y;
}vec2;

typedef struct{
	float x;
	float y;
	float z;

	/*vec3(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
	}*/
}vec3;

typedef struct{
	float r;
	float g;
	float b;
}Color;

uint vec3Hash(vec3 l);

vec3 vec3FromHash(uint hash);

template<typename N, size_t s>
size_t arrAmt(N (&)[s]){return s;}

#endif // COMMON_H
