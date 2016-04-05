#include <fstream>
#include <istream>
#include <thread>

#include <common.hpp>
#include <shader_utils.hpp>
#include <ui.hpp>
#include <texture.hpp>
#include <block.hpp>

bool gameRunning = true;

GLuint shaderProgram;
GLint attribute_coord, attribute_t_index;
GLint uniform_mvp, uniform_sampler;
GLuint blockTexture;

glm::vec3 cameraPos;
glm::vec3 cameraRot;
glm::vec3 angle;

GLuint vbo_cube_vertices, vbo_cube_texIndex, vbo_cube_index;

GLuint ibo_cube_elements;

World world;

std::vector<GLuint*>buffersToGen;
bool started;

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

	glEnableVertexAttribArray(attribute_coord);

	cameraPos = glm::vec3(0,2.0,10.0);
	cameraRot = glm::vec3(0,0,0);

	for(float x = -32; x < 32; x+=16)
		for(float y = 0; y < 256; y+=16)
			for(float z = -32; z < 32; z+=16)
				world.createChunk(vec3(x,0,z));
	//world.createChunk(vec3(16,0,16));
	world.updateChunks();

	blockTexture = Texture::loadTexture("assets/blockSheet.png");

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,blockTexture);
	glUniform1f(uniform_sampler,0);

	glClearColor(0.6, 0.8, 1.0, 0.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 1;
}

void render(void){
	cameraRot.x = sinf(angle.x) * cosf(angle.y);
	cameraRot.y = sinf(angle.y);
	cameraRot.z = cosf(angle.x) * cosf(angle.y);

	float ratio = (screen.x >= screen.y ? screen.x / screen.y : screen.y / screen.x);

	glm::mat4 view = glm::lookAt(cameraPos, cameraPos+cameraRot, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*ratio, 0.01f, 2048.0f);

	glm::mat4 mvp = projection * view;

	glUseProgram(shaderProgram);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);
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
}

void logic(){
	static vec3 buf;
	if(true){
		buf.x = floor(cameraPos.x/CHUNK_WIDTH) * CHUNK_WIDTH;
		buf.y = floor(cameraPos.y/CHUNK_HEIGHT) * CHUNK_HEIGHT;
		buf.z = floor(cameraPos.z/CHUNK_DEPTH) * CHUNK_DEPTH;

		unsigned long long hash = vec3Hash(buf);
		Chunk *chunkPtr = nullptr;

		/*chunkPtr = &world.chunk.at(hash);
		if(chunkPtr == nullptr){
			world.createChunk(buf);
			world.updateChunk(buf);
		}*/

		/*for(auto &c : world.chunk){
			if(c.second.hash == hash){
				chunkPtr = &c.second;
			}
		}
		if(chunkPtr == nullptr){
			world.createChunk(buf);
			world.updateChunk(buf);
		}*/

		try{
			world.chunk.at(hash);
		}catch(const std::out_of_range& oor){
			world.createChunk(buf);
			world.updateChunk(buf);
			chunkPtr = world.chunkAt(buf);

			glBindBuffer(GL_ARRAY_BUFFER, chunkPtr->vert_vbo);
			glBufferData(GL_ARRAY_BUFFER, chunkPtr->vertex.size() * sizeof(vec3), &chunkPtr->vertex[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, chunkPtr->tex_vbo);
			glBufferData(GL_ARRAY_BUFFER, chunkPtr->tex_coord.size() * sizeof(vec2), &chunkPtr->tex_coord[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, chunkPtr->vert_vbo_water);
			glBufferData(GL_ARRAY_BUFFER, chunkPtr->vertex_water.size() * sizeof(vec3), &chunkPtr->vertex_water[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, chunkPtr->tex_vbo_water);
			glBufferData(GL_ARRAY_BUFFER, chunkPtr->tex_coord_water.size() * sizeof(vec2), &chunkPtr->tex_coord_water[0], GL_STATIC_DRAW);
		}
	}

}

void callLogic(float *dt){
	static float accum = 0.0f;
	while(true){
		accum += *dt;
		if(accum >= (1000.0f/20.0f)){
			accum = 0;
			logic();
		}
	}
}


void mainLoop(SDL_Window *w){
	started = true;
	static unsigned int prevTime    = 0,	// Used for timing operations
						currentTime = 0;

	static float deltaTime = 0.0f;

	static float beforeRender;

	if(!currentTime)						// Initialize currentTime if it hasn't been
		currentTime = SDL_GetTicks();
	if(!prevTime){
		prevTime=currentTime;
	}

	std::thread(callLogic,&deltaTime).detach();

	while(gameRunning){
		currentTime = SDL_GetTicks();
		deltaTime	= currentTime - prevTime;
		prevTime	= currentTime;

		ui::handleEvents(deltaTime);

		for(auto &b : buffersToGen){
			glGenBuffers(1,b);
		}
		buffersToGen.clear();

		beforeRender = SDL_GetTicks();
		render();
		std::cout << "Time to render: " << SDL_GetTicks() - beforeRender << std::endl;
		SDL_GL_SwapWindow(w);
	}
}

void free_resources(){
	glDeleteProgram(shaderProgram);
	glDeleteBuffers(1, &vbo_cube_vertices);
	glDeleteBuffers(1, &vbo_cube_texIndex);
	glDeleteBuffers(1, &ibo_cube_elements);
}

int main(/*int argc, char *argv[]*/){
	screen = {1280,720};
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
	ui::reshape(screen.x, screen.y);

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
