#include <ui.hpp>

#define MAXTEXWIDTH 1024

static const float sensitivity = .5;

extern bool gameRunning;

extern SDL_Window *window;

extern glm::vec3 cameraPos;
extern glm::vec3 cameraRot;
extern glm::vec3 angle;

extern vec2 screen;

float front = .5;
float side = .3;

bool downMov = false, upMov = false;
bool frontMov = false, backMov = false;
bool rightMov = false, leftMov = false;

static const float movespeed = 10;

namespace ui{
    namespace text{

        struct shader{
            GLuint source;
            GLint attribute_coord;
            GLint uniform_sampler;
            GLint uniform_color;
        }textShader;

        struct textPoint {
        	GLfloat x;
        	GLfloat y;
        	GLfloat s;
        	GLfloat t;
        };
        struct atlas {
        	GLuint tex;		// texture object

        	unsigned int w;			// width of texture in pixels
        	unsigned int h;			// height of texture in pixels

        	struct {
        		float ax;	// advance.x
        		float ay;	// advance.y

        		float bw;	// bitmap.width;
        		float bh;	// bitmap.height;

        		float bl;	// bitmap_left;
        		float bt;	// bitmap_top;

        		float tx;	// x offset of glyph in texture coordinates
        		float ty;	// y offset of glyph in texture coordinates
        	} c[128];		// character information

        	 atlas(FT_Face face, int height) {
        		FT_Set_Pixel_Sizes(face, 0, height);
        		FT_GlyphSlot g = face->glyph;

        		unsigned int roww = 0;
        		unsigned int rowh = 0;
        		 w = 0;
        		 h = 0;

        		 memset(c, 0, sizeof c);

        		/* Find minimum size for a texture holding all visible ASCII characters */
        		for (int i = 32; i < 128; i++) {
        			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
        				fprintf(stderr, "Loading character %c failed!\n", i);
        				continue;
        			}
        			if (roww + g->bitmap.width + 1 >= MAXTEXWIDTH) {
        				w = std::max(w, roww);
        				h += rowh;
        				roww = 0;
        				rowh = 0;
        			}
        			roww += g->bitmap.width + 1;
        			rowh = std::max(rowh, g->bitmap.rows);
        		}

        		w = std::max(w, roww);
        		h += rowh;

        		/* Create a texture that will be used to hold all ASCII glyphs */
        		glActiveTexture(GL_TEXTURE9);
        		glGenTextures(1, &tex);
        		glBindTexture(GL_TEXTURE_2D, tex);
        		glUniform1i(textShader.uniform_sampler, 9);

        		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

        		/* We require 1 byte alignment when uploading texture data */
        		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        		/* Clamping to edges is important to prevent artifacts when scaling */
        		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        		/* Linear filtering usually looks best for text */
        		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        		/* Paste all glyph bitmaps into the texture, remembering the offset */
        		int ox = 0;
        		int oy = 0;

        		rowh = 0;

        		for (int i = 32; i < 128; i++) {
        			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
        				fprintf(stderr, "Loading character %c failed!\n", i);
        				continue;
        			}

        			if (ox + g->bitmap.width + 1 >= MAXTEXWIDTH) {
        				oy += rowh;
        				rowh = 0;
        				ox = 0;
        			}

        			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        			c[i].ax = g->advance.x >> 6;
        			c[i].ay = g->advance.y >> 6;

        			c[i].bw = g->bitmap.width;
        			c[i].bh = g->bitmap.rows;

        			c[i].bl = g->bitmap_left;
        			c[i].bt = g->bitmap_top;

        			c[i].tx = ox / (float)w;
        			c[i].ty = oy / (float)h;

        			rowh = std::max(rowh, g->bitmap.rows);
        			ox += g->bitmap.width + 1;
        		}

        		fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);
        	}

        	~atlas() {
        		glDeleteTextures(1, &tex);
        	}
        };

        FT_Library ft;
        FT_Face face;
        FT_GlyphSlot g;

        GLuint textTex;
        GLuint textVBO;

        atlas *a48;
        atlas *a24;
        atlas *a16;

        int init_text()
        {
            // initialize FreeType
            if (FT_Init_FreeType(&ft)) {
                std::cerr << "Can't initialize FreeType library!" << std::endl;
                return 1;
            }

            // create a new face from the specified file
            if (FT_New_Face(ft, "assets/fonts/coders_crux.ttf", 0, &face)) {
                std::cerr << "Could not load font!" << std::endl;
                return 1;
            }

            // create the text shader
            textShader.source = create_program("text.vert", "text.frag");

            // if it fails, exit
            if (!textShader.source)
                return 1;

            // set all attributes and uniforms for the text shader
            textShader.attribute_coord = get_attrib(textShader.source, "coord");
            textShader.uniform_sampler = get_uniform(textShader.source, "tex");
            textShader.uniform_color = get_uniform(textShader.source, "color");

            // if any of these fail to set, we want to exit
            if (textShader.attribute_coord == -1 || textShader.uniform_sampler == -1 || textShader.uniform_color == -1)
                return 1;

            glGenBuffers(1, &textVBO);

            glUseProgram(textShader.source);

            /* Create texture atlasses for several font sizes */
        	a48 = new atlas(face, 48);
        	a24 = new atlas(face, 24);
        	a16 = new atlas(face, 16);

            GLfloat colors[4] = {
                1.0, 1.0, 1.0, 1.0
            };
            glUniform4fv(textShader.uniform_color, 1, colors);

            glUseProgram(0);

            return 0;
        }

        int render_text(const char *text, float x, float y, float sx, float sy) {
            atlas *a = a24;

            const uint8_t *p;

            glUseProgram(textShader.source);

            /* Use the texture containing the atlas */
            glActiveTexture(GL_TEXTURE9);
            glBindTexture(GL_TEXTURE_2D, a->tex);
            glUniform1i(textShader.uniform_sampler, 9);

            /* Set up the VBO for our vertex data */
            glEnableVertexAttribArray(textShader.attribute_coord);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glVertexAttribPointer(textShader.attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

            textPoint coords[6 * strlen(text)];
            int c = 0;

            /* Loop through all characters */
            for (p = (const uint8_t *)text; *p; p++) {
                /* Calculate the vertex and texture coordinates */
                float x2 = x + a->c[*p].bl * sx;
                float y2 = -y - a->c[*p].bt * sy;
                float w = a->c[*p].bw * sx;
                float h = a->c[*p].bh * sy;

                /* Advance the cursor to the start of the next character */
                x += a->c[*p].ax * sx;
                y += a->c[*p].ay * sy;

                /* Skip glyphs that have no pixels */
                if (!w || !h)
                    continue;

                coords[c++] = (textPoint) {
                    x2, -y2, a->c[*p].tx, a->c[*p].ty
                };
                coords[c++] = (textPoint) {
                    x2 + w, -y2, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty
                };
                coords[c++] = (textPoint) {
                    x2, -y2 - h, a->c[*p].tx, a->c[*p].ty + a->c[*p].bh / a->h
                };
                coords[c++] = (textPoint) {
                    x2 + w, -y2, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty
                };
                coords[c++] = (textPoint) {
                    x2, -y2 - h, a->c[*p].tx, a->c[*p].ty + a->c[*p].bh / a->h
                };
                coords[c++] = (textPoint) {
                    x2 + w, -y2 - h, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty + a->c[*p].bh / a->h
                };
            }

            /* Draw all the character on the screen in one go */
            glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, c);

            glDisableVertexAttribArray(textShader.attribute_coord);
            glUseProgram(0);

            return 0;
        }

        int render_text()
        {

            return 0;
        }
    }

    void reshape(SDL_Window *window, int w, int h)
    {
        (void)window;
		screen.x = w;
        screen.y = h;
        glViewport(0, 0, w, h);
	}

    void handleEvents(SDL_Window *window, float dt)
    {
        static SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    gameRunning = false;
                    break;
                case SDL_WINDOWEVENT:
                    switch (e.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                        std::cout << "Window " << e.window.windowID << " resized to: " << e.window.data1 << ", " << e.window.data2 << std::endl;
                        reshape(window, e.window.data1, e.window.data2);
                        break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    static const float mousespeed = 0.001;

                    angle.x -= e.motion.xrel * mousespeed;
                    angle.y -= e.motion.yrel * mousespeed;

                    if (angle.x < -M_PI)
                        angle.x += M_PI * 2;
                    if (angle.x > M_PI)
                        angle.x -= M_PI * 2;
                    if (angle.y < -M_PI / 2.2)
                        angle.y = -M_PI / 2.2;
                    if (angle.y > M_PI / 2.2)
                        angle.y = M_PI / 2.2;

                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        gameRunning = false;
                    }
                    if (e.key.keysym.sym == SDLK_w) {
                        frontMov = true;
                    }
                    if (e.key.keysym.sym == SDLK_s) {
                        backMov = true;
                    }
                    if (e.key.keysym.sym == SDLK_a) {
                        leftMov = true;
                    }
                    if (e.key.keysym.sym == SDLK_d) {
                        rightMov = true;
                    }
                    if (e.key.keysym.sym == SDLK_SPACE) {
                        upMov = true;
                    }
                    if (e.key.keysym.sym == SDLK_LCTRL) {
                        downMov = true;
                    }
                    if (e.key.keysym.sym == SDLK_LSHIFT) {
                        front*=5.0f;
                        side*=2.0f;
                    }
                    break;
                case SDL_KEYUP:
                    if (e.key.keysym.sym == SDLK_LCTRL) {
                        downMov = false;
                    }
                    if (e.key.keysym.sym == SDLK_SPACE) {
                        upMov = false;
                    }
                    if (e.key.keysym.sym == SDLK_w) {
                        frontMov = false;
                    }
                    if (e.key.keysym.sym == SDLK_s) {
                        backMov = false;
                    }
                    if (e.key.keysym.sym == SDLK_a) {
                        leftMov = false;
                    }
                    if (e.key.keysym.sym == SDLK_d) {
                        rightMov = false;
                    }
                    if (e.key.keysym.sym == SDLK_LSHIFT) {
                        front/=5.0f;
                        side/=2.0f;
                    }

                default:
                    break;

            }
        }
        if (upMov) {
            cameraPos.y += .025f * front * dt;
        }
        if (downMov) {
            cameraPos.y -= .025f * front * dt;
        }
        if (frontMov) {
            cameraPos.x += float(sin(angle.x))*.025f*front*dt;
            cameraPos.z += float(cos(angle.x))*.025f*front*dt;
        }
        if (backMov) {
            cameraPos.x -= float(sin(angle.x))*.025f*front*dt;
            cameraPos.z -= float(cos(angle.x))*.025f*front*dt;
        }
        if (leftMov) {
            cameraPos.x += float(cos(angle.x))*.025f*side*dt;
            cameraPos.z -= float(sin(angle.x))*.025f*side*dt;
        }
        if (rightMov) {
            cameraPos.x -= float(cos(angle.x))*.025f*side*dt;
            cameraPos.z += float(sin(angle.x))*.025f*side*dt;
        }

    }
}
