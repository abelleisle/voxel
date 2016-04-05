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

static int init_resources(){
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

	for(float x = -64; x < 64; x+=16)
		for(float z = -64; z < 64; z+=16)
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
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);

	//glCullFace(GL_BACK);

	glUseProgram(shaderProgram);
	glEnableVertexAttribArray(attribute_coord);
	glEnableVertexAttribArray(attribute_t_index);

	for(auto &c : world.chunk){
		c.second.render();
	}

	glDisableVertexAttribArray(attribute_coord);
	glDisableVertexAttribArray(attribute_t_index);
}

void mainLoop(SDL_Window *w){
	static unsigned int prevTime    = 0,	// Used for timing operations
						currentTime = 0;

	static float deltaTime = 0;

	if(!currentTime)						// Initialize currentTime if it hasn't been
		currentTime = SDL_GetTicks();
	if(!prevTime){
		prevTime=currentTime;
	}


	while(gameRunning){
		currentTime = SDL_GetTicks();
		deltaTime	= currentTime - prevTime;
		prevTime	= currentTime;

		std::cout << 1000.0f/deltaTime << std::endl;

		ui::handleEvents(deltaTime);

		render();
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
