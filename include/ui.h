#ifndef UI_H
#define UI_H

#include <common.h>

static const float sensitivity = .5;

extern bool gameRunning;

extern SDL_Window *window;

extern vec3 cameraPos;
extern vec2 cameraRot;

extern unsigned int SCREEN_HEIGHT;
extern unsigned int SCREEN_WIDTH;

extern Player player;

const float front = .5;
const float side = .5;

namespace ui{
	void handleEvents(){
		//const Uint8 *key = SDL_GetKeyboardState(NULL);
		static SDL_Event e;
		static float yrotrad;
		// static int mx, my;
		// static int dx, dy;
		// SDL_GetMouseState(&mx,&my);
		// dx = mx - SCREEN_WIDTH/2;
		// dy = my - SCREEN_HEIGHT/2;
		// SDL_WarpMouseInWindow(window,SCREEN_WIDTH/2,SCREEN_HEIGHT/2);

		// std::cout << dx << ", " << dy << std::endl;
		//cameraRot.x += dy;
		//cameraRot.y += dx;
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:
					gameRunning = false;
					break;
				case SDL_MOUSEMOTION:
					cameraRot.x += float(e.motion.yrel)*.25*sensitivity;
					cameraRot.y += float(e.motion.xrel)*.25*sensitivity;
					if(cameraRot.x < -85)cameraRot.x = -85;
					if(cameraRot.x > 85)cameraRot.x = 85;
					if(cameraRot.y >= 360)cameraRot.y = cameraRot.y-360;
					if(cameraRot.y < 0) cameraRot.y = 360 - cameraRot.y;
					break;
				case SDL_KEYDOWN:
					if(e.key.keysym.sym == SDLK_ESCAPE){
						gameRunning = false;
					}
					if(e.key.keysym.sym == SDLK_w){
						yrotrad = (cameraRot.y / 180 * 3.141592654f);
						//std::cout << yrotrad << std::endl;
					    player.loc.x += float(sin(yrotrad))*front;
					    player.loc.z -= float(cos(yrotrad))*front;
					}
					if(e.key.keysym.sym == SDLK_s){
						yrotrad = (cameraRot.y / 180 * 3.141592654f);
						player.loc.x -= float(sin(yrotrad))*front;
						player.loc.z += float(cos(yrotrad))*front;
					}
					if(e.key.keysym.sym == SDLK_a){
						yrotrad = (cameraRot.y / 180 * 3.141592654f);
						player.loc.x -= float(cos(yrotrad))*side;
						player.loc.z -= float(sin(yrotrad))*side;
					}
					if(e.key.keysym.sym == SDLK_d){
						yrotrad = (cameraRot.y / 180 * 3.141592654f);
						player.loc.x += float(cos(yrotrad))*side;
						player.loc.z += float(sin(yrotrad))*side;
					}
					if(e.key.keysym.sym == SDLK_SPACE){
						player.loc.y+=.5;
					}
					if(e.key.keysym.sym == SDLK_LCTRL){
						player.loc.y-=.5;
					}
					break;
			}
		}
	}
}

#endif //UI_H
