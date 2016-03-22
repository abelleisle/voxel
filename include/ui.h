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

namespace ui{
	void handleEvents(){
		const Uint8 *key = SDL_GetKeyboardState(NULL);
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
			if(e.type == SDL_QUIT){
				gameRunning = false;
			}
			if(e.type == SDL_MOUSEMOTION){
				cameraRot.x += float(e.motion.yrel)*.25*sensitivity;
				cameraRot.y += float(e.motion.xrel)*.25*sensitivity;
				if(cameraRot.x < -85)cameraRot.x = -85;
				if(cameraRot.x > 85)cameraRot.x = 85;
			}
			if(key[SDL_SCANCODE_ESCAPE]) gameRunning = false;
			if(key[SDL_SCANCODE_W]){
			    yrotrad = (cameraRot.y / 180 * 3.141592654f);
			    player.loc.x += float(sin(yrotrad))*.05;
			    player.loc.z -= float(cos(yrotrad))*.05;
			}
			if(key[SDL_SCANCODE_S]){
				yrotrad = (cameraRot.y / 180 * 3.141592654f);
				player.loc.x -= float(sin(yrotrad))*.05;
				player.loc.z += float(cos(yrotrad))*.05;
			}
			if(key[SDL_SCANCODE_A]){
				yrotrad = (cameraRot.y / 180 * 3.141592654f);
				player.loc.x -= float(cos(yrotrad))*.01;
				player.loc.z -= float(sin(yrotrad))*.01;
			}
			if(key[SDL_SCANCODE_D]){
				yrotrad = (cameraRot.y / 180 * 3.141592654f);
				player.loc.x += float(cos(yrotrad))*.01;
				player.loc.z += float(sin(yrotrad))*.01;
			}
			if(key[SDL_SCANCODE_SPACE]){
				player.loc.y+=.5;
			}
			if(key[SDL_SCANCODE_LCTRL]){
				player.loc.y-=.1;
			}
			if(key[SDL_SCANCODE_P]){
				player.breaku = true;
			}
		}
	}
}

#endif //UI_H