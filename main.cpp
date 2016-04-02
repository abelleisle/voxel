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

	for(float x = -128; x < 128; x+=16)
		for(float z = -128; z < 128; z+=16)
			world.createChunk(vec3(x,0,z));
	world.updateChunks();

	blockTexture = Texture::loadTexture("assets/blockSheet.png");

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,blockTexture);
	glUniform1f(uniform_sampler,0);


	glClearColor(0.6, 0.8, 1.0, 0.0);
	//std::cout << blockIndex(1,2) << std::endl;

	GLfloat cube_vertices[] = {
		// front
		-1.0, -1.0,  1.0, // bottom left // 0
		 1.0, -1.0,  1.0, // bottom right
		 1.0,  1.0,  1.0, // top right
		-1.0,  1.0,  1.0, // top left

		// back
		-1.0, -1.0, -1.0, // bottom left // 4
		 1.0, -1.0, -1.0, // bottom right
		 1.0,  1.0, -1.0, // top right
		-1.0,  1.0, -1.0, // top left

		// left
		-1.0, -1.0, -1.0,  // bottom left // 8
		-1.0, -1.0,  1.0,  // bottom right
		-1.0,  1.0,  1.0,  // top right
		-1.0,  1.0, -1.0,  // top left

		// right
		 1.0, -1.0, -1.0, // bottom left // 12
		 1.0, -1.0,  1.0, // bottom right
		 1.0,  1.0,  1.0, // top right
		 1.0,  1.0, -1.0, // top left

		 // top
 		-1.0,  1.0,  1.0, // bottom left // 16
 		 1.0,  1.0,  1.0, // bottom right
 		 1.0,  1.0, -1.0, // top right
 		-1.0,  1.0, -1.0, // top left

		 // bottom
 		-1.0, -1.0,  1.0, // bottom left // 20
 		 1.0, -1.0,  1.0, // bottom right
 		 1.0, -1.0, -1.0, // top right
 		-1.0, -1.0, -1.0, // top left
	};
	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	GLfloat cube_tex[] = {
		// front coords
		0.0, 0.33,
		0.33, 0.33,
		0.33, 0.66,
		0.0, 0.66,

		// back coords
		0.0, 0.33,
		0.33, 0.33,
		0.33, 0.66,
		0.0, 0.66,

		// left coords
		0.0, 0.33,
		0.33, 0.33,
		0.33, 0.66,
		0.0, 0.66,

		// right coords
		0.0, 0.33,
		0.33, 0.33,
		0.33, 0.66,
		0.0, 0.66,

		// top coords
		0.0, 0.66,
		0.33, 0.66,
		0.33, 1.0,
		0.0, 1.0,

		// bottom coords
		0.0, 0.0,
		0.33, 0.0,
		0.33, 0.33,
		0.0, 0.33,
	};
	glGenBuffers(1, &vbo_cube_texIndex);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texIndex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);

	GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// back
		4, 5, 6,
		6, 7, 4,
		// left
		8, 9, 10,
		10, 11, 8,
		// right
		12, 13, 14,
		14, 15, 12,
		// top
		16, 17, 18,
		18, 19, 16,
		// bottom
		20, 21, 22,
		22, 23, 20,
	};

	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	return 1;
}

void render(void){
	cameraRot.x = sinf(angle.x) * cosf(angle.y);
	cameraRot.y = sinf(angle.y);
	cameraRot.z = cosf(angle.x) * cosf(angle.y);

	float ratio = (screen.x >= screen.y ? screen.x / screen.y : screen.y / screen.x);

	glm::mat4 view = glm::lookAt(cameraPos, cameraPos+cameraRot, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*ratio, 0.01f, 1000.0f);

	glm::mat4 mvp = projection * view;

	glUseProgram(shaderProgram);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glEnableVertexAttribArray(attribute_coord);
	glEnableVertexAttribArray(attribute_t_index);


	/*// Describe our vertices array to OpenGL (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glVertexAttribPointer(
		attribute_coord, // attribute
		3,                 // number of elements per vertex, here (x,y,z,w)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);

	glEnableVertexAttribArray(attribute_t_index);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texIndex);
	glVertexAttribPointer(
		attribute_t_index, // attribute
		2,                 // number of elements per vertex, here (x,y)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);*/

	/* Push each element in buffer_vertices to the vertex shader */
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);*/

	for(auto &c : world.chunk){
		c.second.render();
	}

	glDisableVertexAttribArray(attribute_coord);
	glDisableVertexAttribArray(attribute_t_index);
}

void mainLoop(SDL_Window *w){
	while(gameRunning){
		ui::handleEvents();

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
