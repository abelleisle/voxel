#include <ui.hpp>

static const float sensitivity = .5;

extern bool gameRunning;

extern SDL_Window *window;

extern glm::vec3 cameraPos;
extern glm::vec3 cameraRot;
extern glm::vec3 angle;

const float front = .5;
const float side = .3;

bool downMov = false, upMov = false;
bool frontMov = false, backMov = false;
bool rightMov = false, leftMov = false;

static const float movespeed = 10;

namespace ui{
    void reshape(int w, int h){
        screen.x = w;
        screen.y = h;
        glViewport(0, 0, w, h);
    }

    void handleEvents(){
        static SDL_Event e;
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case SDL_QUIT:
                    gameRunning = false;
                    break;
                case SDL_WINDOWEVENT:
                    if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                        reshape(e.window.data1,e.window.data2);
                    break;
                case SDL_MOUSEMOTION:
                    static const float mousespeed = 0.001;

                    angle.x -= e.motion.xrel * mousespeed;
                    angle.y -= e.motion.yrel * mousespeed;

                    if(angle.x < -M_PI)
                        angle.x += M_PI * 2;
                    if(angle.x > M_PI)
                        angle.x -= M_PI * 2;
                    if(angle.y < -M_PI / 2)
                        angle.y = -M_PI / 2;
                    if(angle.y > M_PI / 2)
                        angle.y = M_PI / 2;

                    break;
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_ESCAPE){
                        gameRunning = false;
                    }
                    if(e.key.keysym.sym == SDLK_w){
                        frontMov = true;
                    }
                    if(e.key.keysym.sym == SDLK_s){
                        backMov = true;
                    }
                    if(e.key.keysym.sym == SDLK_a){
                        leftMov = true;
                    }
                    if(e.key.keysym.sym == SDLK_d){
                        rightMov = true;
                    }
                    if(e.key.keysym.sym == SDLK_SPACE){
                        upMov = true;
                    }
                    if(e.key.keysym.sym == SDLK_LCTRL){
                        downMov = true;
                    }
                    if(e.key.keysym.sym == SDLK_LSHIFT){
                        std::cout << "Shift" << std::endl;
                    }
                    break;
                case SDL_KEYUP:
                    if(e.key.keysym.sym == SDLK_LCTRL){
                        downMov = false;
                    }
                    if(e.key.keysym.sym == SDLK_SPACE){
                        upMov = false;
                    }
                    if(e.key.keysym.sym == SDLK_w){
                        frontMov = false;
                    }
                    if(e.key.keysym.sym == SDLK_s){
                        backMov = false;
                    }
                    if(e.key.keysym.sym == SDLK_a){
                        leftMov = false;
                    }
                    if(e.key.keysym.sym == SDLK_d){
                        rightMov = false;
                    }

                default:
                    break;

            }
        }
        if(upMov){
            cameraPos.y += .05 * front;
        }
        if(downMov){
            cameraPos.y -= .05 * front;
        }
        if(frontMov){
            cameraPos.x += float(sin(angle.x))*.05*front;
            cameraPos.z += float(cos(angle.x))*.05*front;
        }
        if(backMov){
            cameraPos.x -= float(sin(angle.x))*.05*front;
            cameraPos.z -= float(cos(angle.x))*.05*front;
        }
        if(leftMov){
            cameraPos.x += float(cos(angle.x))*.05*side;
            cameraPos.z -= float(sin(angle.x))*.05*side;
        }
        if(rightMov){
            cameraPos.x -= float(cos(angle.x))*.05*side;
            cameraPos.z += float(sin(angle.x))*.05*side;
        }

    }
}
