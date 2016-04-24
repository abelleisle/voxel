#include <fstream>
#include <istream>
#include <thread>

#include <common.hpp>
#include <ui.hpp>
#include <texture.hpp>
#include <block.hpp>

// game running determines when the logic loop needs to stop
bool gameRunning = true;
// once logic stops running, we can stop the render loop
bool logicIsDone = false;

// stores the shaders information
GLuint shaderProgram;
// stores the locations of the attributes in the shader
GLint attribute_coord, attribute_t_index;
// stores the location of the uniforms in the shader
GLint uniform_mvp, uniform_sampler;
// store the location of the block texture sheet we use
GLuint blockTexture;

// cameras location and rotation
glm::vec3 cameraPos;
glm::vec3 cameraRot;
glm::vec3 angle;

// our world variable
// TODO, change this
World world;

/*
 *	Global variables used to store our buffers for the main loop
 *	to generate. We need to do this because opengl's context is
 *	in the main thread.
 */
std::vector<GLuint*>buffersToGen;

/*
 *	Global variable for storing chunks that need the main thread
 *	to do certain operations. ie: opengl, or sdl operations.
 */
std::vector<Chunk*>chunkPtrs;

// tells us if we have started the main loop
bool started;

std::mutex threadMtx;

static int init_resources(){
	started = false;
	shaderProgram = create_program("frig.vert","frig.frag");

	if(!shaderProgram)
		return 0;

	attribute_coord = get_attrib(shaderProgram, "coord");
	attribute_t_index = get_attrib(shaderProgram, "textureCoord");
	uniform_mvp = get_uniform(shaderProgram, "mvp");
	uniform_sampler = get_uniform(shaderProgram, "texture");

	if(attribute_coord == -1 || uniform_mvp == -1)
		return 0;

	if(ui::text::init_text()){
		std::cerr << "Could not initialize fonts!" << std::endl;
		return 0;
	}

	glEnableVertexAttribArray(attribute_coord);

	cameraPos = glm::vec3(0,2.0,10.0);
	cameraRot = glm::vec3(0,0,0);

	// for(float x = -128; x <= 128; x+=16)
	// 		for(float z = -128; z <= 128; z+=16)
	// 			world.createChunk(vec3(x,0,z));
	// //world.createChunk(vec3(16,0,16));
	// world.updateChunks();

	blockTexture = Texture::loadTexture("assets/blockSheet.png");

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,blockTexture);
	glUniform1f(uniform_sampler,0);

	glClearColor(0.6, 0.8, 1.0, 0.0);

	return 1;
}

void render(void){
	cameraRot.x = sinf(angle.x) * cosf(angle.y);
	cameraRot.y = sinf(angle.y);
	cameraRot.z = cosf(angle.x) * cosf(angle.y);

	//float ratio = (screen.x >= screen.y ? screen.x / screen.y : screen.y / screen.x);

	glm::mat4 view = glm::lookAt(cameraPos, cameraPos+cameraRot, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*(screen.x / screen.y), 0.01f, 2048.0f);
	//glm::mat4 projection = glm::ortho(0,screen.x,0,screen.y,0.01f, 2048.0f);

	glm::mat4 mvp = projection * view;

	glUseProgram(shaderProgram);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//glUseProgram(shaderProgram);
	glEnableVertexAttribArray(attribute_coord);
	glEnableVertexAttribArray(attribute_t_index);

	for(auto &c : world.chunk){
		c.second.render();
	}
	for(auto &c : world.chunk){
		c.second.renderL();
	}

	glDisableVertexAttribArray(attribute_coord);
	glDisableVertexAttribArray(attribute_t_index);

	glUseProgram(0);
}

void logic(){
	static vec3 buf;

	buf.x = floor(cameraPos.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	// use this if world height is made up of multiple chunks
	//buf.y = floor(cameraPos.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	// use this if world height is only made up of 1 chunk
	buf.y = 0;
	buf.z = floor(cameraPos.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	unsigned long long hash = vec3Hash(buf);

	try{
		world.chunk.at(hash);
	}catch(const std::out_of_range& oor){
		world.createChunk(buf);
		world.updateChunk(buf);

		//chunkPtrs.push_back(world.chunkAt(buf));
		world.updateChunk(vec3(buf.x+CHUNK_WIDTH,buf.y,buf.z));
		world.updateChunk(vec3(buf.x-CHUNK_WIDTH,buf.y,buf.z));
		world.updateChunk(vec3(buf.x,buf.y,buf.z+CHUNK_DEPTH));
		world.updateChunk(vec3(buf.x,buf.y,buf.z-CHUNK_DEPTH));

		//threadMtx.lock();
		//threadMtx.unlock();
	}

	/*for(auto &c : world.chunk){
		if(sqrt(pow(c.second.loc.x - buf.x,2) + pow(c.second.loc.z - buf.z,2)) > 64){
			world.chunk.erase(world.chunk.find(c.second.hash));
		}
	}*/

	/*poss = cameraPos;

	poss.x = floor(poss.x/CHUNK_WIDTH) * CHUNK_WIDTH;
	poss.y = floor(poss.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
	poss.z = floor(poss.z/CHUNK_DEPTH) * CHUNK_DEPTH;

	poss.x -= CHUNK_WIDTH * CHUNK_RAD;
	poss.z -= CHUNK_DEPTH * CHUNK_RAD;
	poss.y = cameraPos.y;

	for(float x = poss.x; x < poss.x + CHUNK_WIDTH*(CHUNK_RAD+1);x+=CHUNK_WIDTH){
		for(float z = poss.z; z < poss.z + CHUNK_DEPTH*(CHUNK_RAD+1);z+=CHUNK_DEPTH){

		}
	}*/
}

void callLogic(float *dt){
	ThreadPool chunkCreate(50);
	for (float x = -128; x <= 128; x+=16)
		for (float z = -128; z <= 128; z+=16)
			if (gameRunning) {
				chunkCreate.Enqueue([&]{
					world.createChunk(vec3(x,0,z));
					world.updateChunk(vec3(x,0,z));

					world.updateChunk(vec3(x - 16, 	0, z	 ));
					world.updateChunk(vec3(x + 16, 	0, z	 ));
					world.updateChunk(vec3(x,		0, z - 16));
					world.updateChunk(vec3(x,		0, z + 16));
				});
			}

	static float accum = 0.0f;
	while (gameRunning) {
		accum += *dt;
		if (accum >= (1000.0f/20.0f)) {
			accum = 0;
			logic();
		}
	}
	logicIsDone = true;
}


void mainLoop(SDL_Window *w){
	started = true;
	static unsigned int prevTime    = 0,	// Used for timing operations
						currentTime = 0;

	static float deltaTime = 0.0f;

	static float beforeRender;

	static float fpsTime;
	static float fps;

	static float renderTime = 0.0f;
	static float renderTimeAv = 0.0f;
	static int renderLoops = 0;

	if(!currentTime)						// Initialize currentTime if it hasn't been
		currentTime = SDL_GetTicks();
	if(!prevTime){
		prevTime=currentTime;
	}

	std::thread(callLogic,&deltaTime).detach();

	while(!logicIsDone){
		currentTime = SDL_GetTicks();
		deltaTime	= currentTime - prevTime;
		prevTime	= currentTime;

		ui::handleEvents(w, deltaTime);

		threadMtx.lock();
		for(auto &b : buffersToGen){
			glGenBuffers(1,b);
		}
		buffersToGen.clear();

		for(auto &chunkPtr : chunkPtrs){
			glBindBuffer(GL_ARRAY_BUFFER, chunkPtr->vert_vbo);
			glBufferData(GL_ARRAY_BUFFER, chunkPtr->vertex.size() * sizeof(vec3), &chunkPtr->vertex[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, chunkPtr->tex_vbo);
			glBufferData(GL_ARRAY_BUFFER, chunkPtr->tex_coord.size() * sizeof(vec2), &chunkPtr->tex_coord[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, chunkPtr->vert_vbo_water);
			glBufferData(GL_ARRAY_BUFFER, chunkPtr->vertex_water.size() * sizeof(vec3), &chunkPtr->vertex_water[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, chunkPtr->tex_vbo_water);
			glBufferData(GL_ARRAY_BUFFER, chunkPtr->tex_coord_water.size() * sizeof(vec2), &chunkPtr->tex_coord_water[0], GL_STATIC_DRAW);

			chunkPtr->canRender = true;
		}
		chunkPtrs.clear();
		threadMtx.unlock();

		float sx = 2.0 / screen.x;
		float sy = 2.0 / screen.y;

		beforeRender = SDL_GetTicks();

		render();
		ui::text::render_text(std::string("FPS: " + std::to_string(int(fps))).c_str(), -1 + 8 * sx, 1 - 16 * sy, sx, sy);
		ui::text::render_text(std::string("Pos: " + std::to_string(cameraPos.x) + ", " + std::to_string(cameraPos.y) + ", " + std::to_string(cameraPos.z)).c_str(), -1 + 8 * sx, 1 - 36 * sy, sx, sy);
		ui::text::render_text(std::string("Time to render: " + std::to_string(int(renderTimeAv))).c_str(), -1 + 8 * sx, 1 - 56 * sy, sx, sy);

		fpsTime += deltaTime;
		renderTime += int(SDL_GetTicks() - beforeRender);
		renderLoops++;
		if(fpsTime >= 250){
			fps = ((1000 * renderLoops) / fpsTime);
			fpsTime = 0.0f;

			renderTimeAv = renderTime / renderLoops;
			renderTime = 0.0f;

			renderLoops = 0;
		}
		SDL_GL_SwapWindow(w);
	}
}

void free_resources(){
	glDeleteProgram(shaderProgram);
}

int main(/*int argc, char *argv[]*/){
	screen = {1920,1080};
	SDL_Init(SDL_INIT_VIDEO);

	// Select an OpenGL ES 2.0 profile.
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_Window *window = SDL_CreateWindow("Voxel Engine",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen.x, screen.y,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum err;
	glewExperimental = GL_TRUE;
	if((err=glewInit()) != GLEW_OK){
		std::cout << "GLEW was not able to initialize! Error: " << glewGetErrorString(err) << std::endl;
		return -1;
	}

	// Set relative mouse mode, this will grab the cursor.
	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_GL_SetSwapInterval(0);
	ui::reshape(window, screen.x, screen.y);

	print_opengl_info();

	if (!init_resources())
		return EXIT_FAILURE;

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	mainLoop(window);

	free_resources();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
	SDL_Quit();

    return 0; // Calls everything passed to atexit
}
