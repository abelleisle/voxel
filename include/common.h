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

typedef struct{
	float x;
	float y;
}vec2;

typedef struct{
	float x;
	float y;
	float z;
}vec3;

template<typename N, size_t s>
size_t arrAmt(N (&)[s]){return s;}

#endif // COMMON_H
