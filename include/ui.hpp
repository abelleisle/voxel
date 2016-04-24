#ifndef UI_H
#define UI_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <common.hpp>

namespace ui{
	namespace text{
		int init_text();
		int render_text();
		int render_text(const char *text, float x, float y, float sx, float sy);
	}

	void reshape(SDL_Window *window, int w, int h);
	void handleEvents(SDL_Window *window, float dt);
}

#endif //UI_H
