#ifndef UI_H
#define UI_H

extern bool gameRunning;

extern SDL_Window *window;

extern vec3 cameraPos;
extern vec2 cameraRot;

extern unsigned int SCREEN_HEIGHT;
extern unsigned int SCREEN_WIDTH;

namespace ui{
	void handleEvents(){
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
				cameraRot.x += float(e.motion.yrel)*.25;
				cameraRot.y += float(e.motion.xrel)*.25;
				if(cameraRot.x < -85)cameraRot.x = -85;
				if(cameraRot.x > 85)cameraRot.x = 85;
				break;
			/*case SDL_MOUSEBUTTONDOWN:
				if((e.button.button & SDL_BUTTON_RIGHT) && dialogBoxExists)
					dialogAdvance();
				if((e.button.button & SDL_BUTTON_LEFT) && !dialogBoxExists)
					player->inv->usingi = true;
				break;*/
			case SDL_KEYUP:
				switch(e.key.keysym.sym){
				case SDLK_ESCAPE:
					gameRunning = false;
					break;
				}
				break;
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym){
				case SDLK_LEFT:
					//cameraRot.y-=2;
					//if(cameraRot.y < -360)cameraRot.y+=360;
					break;
				case SDLK_RIGHT:
					//cameraRot.y+=2;
					//if(cameraRot.y > 360)cameraRot.y-=360;
					break;
				case SDLK_DOWN:
					//cameraRot.x--;
					break;
				case SDLK_UP:
					//cameraRot.x++;
					break;
				case SDLK_w:
				    yrotrad = (cameraRot.y / 180 * 3.141592654f);
				    cameraPos.x += float(sin(yrotrad))*.2;
				    cameraPos.z -= float(cos(yrotrad))*.2;
					//cameraPos.z+=.5;
					break;
				case SDLK_s:
					yrotrad = (cameraRot.y / 180 * 3.141592654f);
					cameraPos.x -= float(sin(yrotrad))*.2;
					cameraPos.z += float(cos(yrotrad))*.2;
					//cameraPos.z-=.5;
					break;
				case SDLK_a:
					yrotrad = (cameraRot.y / 180 * 3.141592654f);
    				cameraPos.x -= float(cos(yrotrad))*.125;
    				cameraPos.z -= float(sin(yrotrad))*.125;
					//cameraPos.x-=.5;
					break;
				case SDLK_d:
					yrotrad = (cameraRot.y / 180 * 3.141592654f);
    				cameraPos.x += float(cos(yrotrad))*.125;
    				cameraPos.z += float(sin(yrotrad))*.125;
					//cameraPos.x+=.5;
					break;
				case SDLK_SPACE:
					cameraPos.y+=.1;
					break;
				case SDLK_LCTRL:
					cameraPos.y-=.1;
					break;
				}
				break;
			}
		}
	}
}

#endif //UI_H