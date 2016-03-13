/*! @file main.cpp
 *	@brief The file that links everything together for the game to run.
 *	The  main game loop contains all of the global variables the game uses, and it runs the main game loop, the render loop, and the logic loop that control all of the entities.
 */

/*
 * Standard library includes
 */

#include <fstream>
#include <istream>
#include <thread>

#include <common.h>
#include <ui.h>
#include <Block.h>

/**
 * Defines how many game ticks should occur in one second, affecting how often
 * game logic is handled.
 */

#define TICKS_PER_SEC 20

/**
 * Defines how many milliseconds each game tick will take.
 */

#define MSEC_PER_TICK (1000/TICKS_PER_SEC)

/**
 * The window object returned by SDL when we create the main window.
 */

SDL_Window *window = NULL;

/**
 * Determines when the game should exit. This variable is set to true right
 * before the main loop is entered, once set to false the game will exit/
 * free resources.
 */

bool gameRunning;

/**
 * Used for texture animation. It is externally referenced by ui.cpp
 * and entities.cpp.
 */

unsigned int loops = 0;

#define GAME_NAME "Voxel Engine"

unsigned int SCREEN_WIDTH = 1280;
unsigned int SCREEN_HEIGHT = 720;
bool FULLSCREEN;

float VOLUME_MASTER;
float VOLUME_MUSIC;
float VOLUME_SFX;

vec3 cameraPos;
vec2 cameraRot;

/**
 * The game logic function, should handle all logic-related operations for the
 * game.
 */

void logic(void);

/**
 * The game render function, should handle all drawing to the window.
 */

void render(void);

/**
 * The main loop, calls logic(), render(), and does timing operations in the
 * appropriate order.
 */

void mainLoop(void);



/*******************************************************************************
 * MAIN ************************************************************************
 *******************************************************************************/
int main(/*int argc, char *argv[]*/){
	// *argv = (char *)argc;
	SDL_GLContext mainGLContext = NULL;
	
	gameRunning=false;

	/**
	 * (Attempt to) Initialize SDL libraries so that we can use SDL facilities and eventually
	 * make openGL calls. Exit if there was an error.
	 */

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
		std::cout << "SDL was not able to initialize! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	// Run SDL_Quit when main returns
	atexit(SDL_Quit);

	/**
	 * (Attempt to) Initialize SDL_image libraries with IMG_INIT_PNG so that we can load PNG
	 * textures for the entities and stuff.
	 */

	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
		std::cout << "Could not init image libraries! Error: " << IMG_GetError() << std::endl;
		return -1;
	}

	// Run IMG_Quit when main returns
	atexit(IMG_Quit);

	/**
	 * (Attempt to) Initialize SDL_mixer libraries for loading and playing music/sound files.
	 */

	if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
		std::cout << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
		return -1;
	}
	Mix_AllocateChannels(8);

	// Run Mix_Quit when main returns
	atexit(Mix_Quit);

	/*
	 *	Create a window for SDL to draw to. Most parameters are the default, except for the
	 *	following which are defined in include/common.h:
	 *
	 *	GAME_NAME		the name of the game that is displayed in the window title bar
	 *	SCREEN_WIDTH	the width of the created window
	 *	SCREEN_HEIGHT	the height of the created window
	 *	FULLSCREEN		makes the window fullscreen
	 *
	 */

	uint32_t SDL_CreateWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | (FULLSCREEN ? SDL_WINDOW_FULLSCREEN : 0);

	window = SDL_CreateWindow(GAME_NAME,
							  SDL_WINDOWPOS_UNDEFINED,	// Spawn the window at random (undefined) x and y coordinates
							  SDL_WINDOWPOS_UNDEFINED,	//
							  SCREEN_WIDTH,
							  SCREEN_HEIGHT,
							  SDL_CreateWindowFlags
							  );

    /*
     * Exit if the window cannot be created
     */

    if(window==NULL){
		std::cout << "The window failed to generate! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    /*
     * Create the SDL OpenGL context. Once created, we are allowed to use OpenGL functions.
     * Saving this context to mainGLContext does not appear to be necessary as mainGLContext
     * is never referenced again.
     */

    if((mainGLContext = SDL_GL_CreateContext(window)) == NULL){
		std::cout << "The OpenGL context failed to initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

	/*
	 * Initialize GLEW libraries, and exit if there was an error.
	 * Not sure what they're for yet.
	 */

	GLenum err;
#ifndef __WIN32__
	glewExperimental = GL_TRUE;
#endif
	if((err=glewInit()) != GLEW_OK){
		std::cout << "GLEW was not able to initialize! Error: " << glewGetErrorString(err) << std::endl;
		return -1;
	}

	/*
	 * Do some basic setup for openGL. Enable double buffering, switch to by-pixel coordinates,
	 * setup the alpha channel for textures/transparency, and finally hide the system's mouse
	 * cursor so that we may draw our own.
	 */
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(0);
	
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	SDL_SetWindowGrab(window,SDL_TRUE);
	//SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	/**************************
	****     GAMELOOP      ****
	**************************/
	
	std::cout << "Num threads: " << std::thread::hardware_concurrency() << std::endl;

	cameraRot.x = 0;
	cameraRot.y = 0;

	cameraPos.x = 0;
	cameraPos.y = 0;
	cameraPos.z = 0;

	//int meme = 0;

	World world;
	world.createChunk({0,0,0});
	for(auto &c : world.chunk){
		for(uint h = 0; h < CHUNK_HEIGHT; h++){
			for(uint w = 0; w < CHUNK_WIDTH; w++){
				for(uint d = 0; d < CHUNK_DEPTH; d++){
					if(32 != 0)
						c.block[h][w][d].color = {120,72,0};
					else
						c.block[h][w][d].color = {25,255,25};
				}
			}
		}
	}

	gameRunning = true;
	while(gameRunning){
		mainLoop();
	}
	
	/**************************
	****   CLOSE PROGRAM   ****
	**************************/
	
    /*
     * Close the window and free resources
     */
    
    Mix_HaltMusic();
    Mix_CloseAudio();

    SDL_GL_DeleteContext(mainGLContext);
    SDL_DestroyWindow(window);
    
    return 0; // Calls everything passed to atexit
}

void mainLoop(void){
	static float deltaTime;
	static unsigned int prevTime = 0;
	static unsigned int prevPrevTime= 0,	// Used for timing operations
						currentTime = 0;	//
	
	if(!currentTime)						// Initialize currentTime if it hasn't been
		currentTime=SDL_GetTicks();
	
	/*
	 * Update timing values. This is crucial to calling logic and updating the window (basically
	 * the entire game).
	 */
	
	prevTime	= currentTime;
	currentTime = SDL_GetTicks();
	deltaTime	= currentTime - prevTime;

	ui::handleEvents();
	/*
	 * Run the logic handler if MSEC_PER_TICK milliseconds have passed.
	 */

	if(prevPrevTime + MSEC_PER_TICK <= currentTime){
		logic();
		prevPrevTime = currentTime;
	}
	std::cout << 1000/deltaTime << std::endl;
	//std::cout << "Cam: " << cameraPos.x << "," << cameraPos.y << "," << cameraPos.z << std::endl;
	render();
	
}

void perspectiveGl(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar){
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;

	// fH = tan((fovY/2)/180*pi)*zNear;
	fH = tan(fovY / 360 * pi) * zNear;
	fW = fH * aspect;

	glFrustum(-fW,fW,-fH,fH,zNear,zFar);
}

void render(){
	/*
	 *	These functions run everyloop to update the current stacks presets
	 *
	 *	Matrix 	----	A matrix is a blank "canvas" for the renderer to draw on,
	 *					this canvas can be rotated, scales, skewed, etc..
	 *
	 *	Stack 	----	A stack is exactly what it sounds like, it is a stack.. A
	 *					stack is a "stack" of matrices for the renderer to draw on.
	 *					Each stack can be made up of varying amounts of matricies.
	 *
	 *	glMatrixMode	This changes our current stacks mode so the drawings below
	 *					it can take on certain traits.
	 *	
	 *	GL_PROJECTION	This is the matrix mode that sets the cameras position,
	 *					GL_PROJECTION is made up of a stack with two matrices which
	 *					means we can make up to 2 seperate changes to the camera.
	 *
	 *	GL_MODELVIEW	This matrix mode is set to have the dimensions defined above
	 *					by GL_PROJECTION so the renderer can draw only what the camera
	 *					is looking at. GL_MODELVIEW has a total of 32 matrices on it's
	 *					stack, so this way we can make up to 32 matrix changes like,
	 *					scaling, rotating, translating, or flipping.
	 *
	 *	glOrtho			glOrtho sets our ortho, or our cameras resolution. This can also
	 *					be used to set the position of the camera on the x and y axis
	 *					like we have done. The glOrtho must be set while the stack is in
	 *					GL_PROJECTION mode, as this is the mode that gives the
	 *					camera properties.
	 *
	 *	glPushMatrix	This creates a "new" matrix. What it really does is pull a matrix
	 *					off the bottom of the stack and puts it on the top so the renderer
	 *					can draw on it.
	 *
	 *	glLoadIdentity	This scales the current matrix back to the origin so the
	 *					translations are seen normally on a stack.
	 */
	
	glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	glLoadIdentity();
	//glOrtho(-SCREEN_WIDTH/2,SCREEN_WIDTH/2,-SCREEN_HEIGHT/2,SCREEN_HEIGHT/2,-1000,1000);
	//glOrtho(-640,640,-360,360,1000,-1000);
	//glFrustum(-640,640,-360,360,1000000000,100000);
	perspectiveGl(45.0f,SCREEN_WIDTH/SCREEN_HEIGHT,0.1f,100.0f);
	//glFrustum(-1,1,-1,1,0, 20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	
	/*
	 * glPushAttrib		This passes attributes to the renderer so it knows what it can
	 *					render. In our case, GL_DEPTH_BUFFER_BIT allows the renderer to
	 *					draw multiple objects on top of one another without blending the
	 *					objects together; GL_LIGHING_BIT allows the renderer to use shaders
	 *					and other lighting effects to affect the scene.
	 *
	 * glClear 			This clears the new matrices using the type passed. In our case:
	 *					GL_COLOR_BUFFER_BIT allows the matrices to have color on them
	 */
	
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glRotatef(cameraRot.x,1.0,0.0,0.0);
	glRotatef(cameraRot.y,0.0,1.0,0.0);
	glTranslatef(-cameraPos.x,-cameraPos.y,-cameraPos.z);
	//glScalef(.25f,.25f,.25f);

	/**************************
	**** RENDER STUFF HERE ****
	**************************/
		
	// static float f[] = {0,0,0,		0,255,255,
	// 					100,0,0,	255,255,0,
	// 					100,0,100,	0,0,255,
	// 					0,0,100,	0,255,0,
	// 					50,100,50,	255,0,0};

	// static float t[] = {200,0,0,	255,0,255,
	// 					400,0,0,	0,255,0,
	// 					400,0,200,	0,0,255,
	// 					200,0,200,	255,0,0,
	// 					300,200,100,0,255,0};

	static float f[] = {0,0,0,		0,255,255,
						1,0,0,		255,255,0,
						1,0,-1,		0,0,255,
						0,0,-1,		0,255,0,
						.5,1,-.5,	255,0,0};

	static float t[] = {2,0,0,		255,0,255,
						4,0,0,		0,255,0,
						4,0,-2,		0,0,255,
						2,0,-2,		255,0,0,
						3,2,-1,		255,255,50};

	static unsigned int index[] =  {0,1,2,2,3,0,
									0,1,4,
									1,2,4,
									2,3,4,
									3,0,4};

	/*static float t[] = {0,0,0,
						100,0,0,
						50,100,0};
	static unsigned int ind[] = {0,1,2};*/

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glDepthRange(0,1000000000000000);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3,GL_FLOAT,6*sizeof(float),f);
	glColorPointer(3,GL_FLOAT,6*sizeof(float),&f[3]);
	glDrawElements(GL_TRIANGLES,18,GL_UNSIGNED_INT,index);

	glVertexPointer(3,GL_FLOAT,6*sizeof(float),t);
	glColorPointer(3,GL_FLOAT,6*sizeof(float),&t[3]);
	glDrawElements(GL_TRIANGLES,18,GL_UNSIGNED_INT,index);

	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glColor3ub(255,255,255);
	glBegin(GL_QUADS);
		glVertex3f(5,0,0);
		glVertex3f(6,0,0);
		glVertex3f(6,2,0);
		glVertex3f(5,2,0);
		glVertex3f(5,2,0);
		glVertex3f(6,2,0);
		glVertex3f(6,2,1);
		glVertex3f(5,2,1);
	glEnd();

	/*
	 * These next two function finish the rendering
	 *
	 *	glPopMatrix			This anchors all of the matrices and blends them to a single
	 *						matrix so the renderer can draw this to the screen, since screens
	 *						are only 2 dimensions, we have to combine the matrixes to be 2d.
	 *
	 *  SDL_GL_SwapWindow	Since SDL has control over our renderer, we need to now give our
	 *						new matrix to SDL so it can pass it to the window.
	 */

	// glTranslatef(cameraPos.x,cameraPos.y,cameraPos.z);
	// glRotatef(cameraRot.y,0.0,1.0,0.0);
	// glRotatef(cameraRot.x,1.0,0.0,0.0);

	glPopMatrix();
	SDL_GL_SwapWindow(window);
}

void logic(){

	loops++;
}
