#include <algorithm>
#include <string>

#include <texture.hpp>

/**
 * A structure for keeping track of loaded textures.
 */

typedef struct {
	std::string name;	/**< The file path of the texture.		*/
	GLuint tex;			/**< The GLuint for the loaded texture. */
	vec2 dim;			/**< The dimensions of the texture.		*/
} texture_t;

/**
 * A vector of all loaded textures.
 *
 * Should a texture be asked to be loaded twice, loadTexture() can reference
 * this array and reuse GLuint's to save memory.
 */

static std::vector<texture_t> LoadedTexture;

namespace Texture{
	Color pixels[8][4];

	GLuint loadTexture(std::string fileName){
		SDL_Surface *image;
		GLuint object = 0;

		// check if texture is already loaded
		for(auto &t : LoadedTexture){
			if(t.name == fileName){
				return t.tex;
			}
		}

		// load SDL_surface of texture
		if(!(image = IMG_Load(fileName.c_str())))
			return 0;
		//SDL_DisplayFormatAlpha(image);

		/*
		 * Load texture through OpenGL.
		 */

		glGenTextures(1,&object);				// Turns "object" into a texture
		glBindTexture(GL_TEXTURE_2D,object);	// Binds "object" to the top of the stack
		glPixelStoref(GL_UNPACK_ALIGNMENT,1);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Sets the "min" filter
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// The the "max" filter of the stack

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Wrap the texture to the matrix
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //

		glTexImage2D(GL_TEXTURE_2D,  // Sets the texture to the image file loaded above
					 0,
					 GL_RGBA,
					 image->w,
					 image->h,
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 image->pixels
					 );

		// add texture to LoadedTexture
		LoadedTexture.push_back(texture_t{fileName,object,{(float)image->w,(float)image->h}});

		// free the SDL_Surface
		SDL_FreeSurface(image);

		return object;
	}

	vec2 imageDim(std::string fileName){
		for(auto &t : LoadedTexture){
			if(t.name == fileName)
				return t.dim;
		}
		return {0,0};
	}

	void freeTextures(void){
		while(!LoadedTexture.empty()){
			glDeleteTextures(1, &LoadedTexture.back().tex);
			LoadedTexture.pop_back();
		}
	}
}

Texturec::Texturec(uint amt, ...){
	va_list fNames;
	texState = 0;
	va_start(fNames, amt);
	for(unsigned int i = 0; i < amt; i++)
		image.push_back( Texture::loadTexture(va_arg(fNames, char *)) );
	va_end(fNames);
}

Texturec::Texturec( std::initializer_list<std::string> l )
{
	texState = 0;
	std::for_each( l.begin(), l.end(), [&](std::string s){ image.push_back( Texture::loadTexture( s ) ); });
}

Texturec::Texturec(std::vector<std::string>v){
	texState = 0;
	std::for_each( v.begin(), v.end(), [&](std::string s){ image.push_back( Texture::loadTexture( s ) ); });
}

Texturec::Texturec(uint amt,const char **paths){
	texState = 0;
	for(unsigned int i = 0; i < amt; i++)
		image.push_back( Texture::loadTexture(paths[i]) );
}

Texturec::~Texturec(){
}

void Texturec::bind(unsigned int bn){
	texState = bn;
	glBindTexture(GL_TEXTURE_2D,image[(int)texState]);
}

void Texturec::bindNext(){
	bind(++texState);
}

void Texturec::bindPrev(){
	bind(--texState);
}
