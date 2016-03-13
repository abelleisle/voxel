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

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

const int CHUNK_WIDTH = 32;
const int CHUNK_HEIGHT = 32;
const int CHUNK_DEPTH = 32;

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

template<typename N, size_t s>
size_t arrAmt(N (&)[s]){return s;}

#endif // COMMON_H
