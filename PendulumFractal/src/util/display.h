#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <vector>
#include <string>
/*
some basic functions for SDL2 to make it easier to use

setup:
- call DisplayWindow::init() to create the window
- call DisplayWindow::deinit() to destroy the window
- call DisplayWindow::update() at least once every 
second to keep the window alive

usage:
call DisplayWindow::fill with rgba values as arguments
to make to whole screen that color, update all your 
changes so the user can see them using DisplayWindow::flip()

all functions are based on the SDL2 library, make sure 
to read there if you have any problems or questions.
*/
class DisplayWindow {
public:
	//update key presses and keep the window alive
	static void update();
	//update the screen for the user
	static void flip();

	static SDL_Texture* loadTexture(std::string path);
	static void saveTextureToFile(const char* file_name, SDL_Texture* texture);
	static SDL_Texture* createTextureRGBA(int width, int height);
	//draw texture on the screen
	static void copyTextureToRenderTarget(SDL_Texture* texture, SDL_Rect* src_rect, SDL_Rect* dest_rect);
	static void copyTextureToRenderTarget(SDL_Texture* texture, SDL_Rect* src_rect, SDL_Rect* dest_rect, double angle, SDL_Point* center, SDL_RendererFlip flip);
	static void setRenderTarget(SDL_Texture* texture) { SDL_SetRenderTarget(renderer, texture); }
	static void destroyTexture(SDL_Texture* texture);

	static SDL_Surface* loadSurface(std::string path);
	static void destroySurface(SDL_Surface* surface);

	static void drawRectangle(int x, int y, int w, int h, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	static void fill(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	static Uint32 getPixel32(SDL_Surface* surface, int x, int y);

	static inline SDL_Rect createSDLRect(int x, int y, int w, int h) { SDL_Rect r; r.x = x; r.y = y; r.w = w; r.h = h; return r; }
	static inline void clearRenderer() { SDL_RenderClear(renderer); }
	static inline void setRenderDrawColour(unsigned char r, unsigned char g, unsigned char b, unsigned char alpha) { SDL_SetRenderDrawColor(renderer, r, g, b, alpha); }
	static inline void setTextureToBlend(SDL_Texture* texture) { SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND); }
	static inline void setTextureToOverwrite(SDL_Texture* texture) { SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE); }

	static inline bool isLeftMouseButtonPressed() { return(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)); }
	static inline bool isRightMouseButtonPressed() { return(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)); }
	static int getMouseXPos();
	static int getMouseYPos();
	static const Uint8* getKeyboardState();

	static void init(int width, int height);
	static void deinit();
	static bool isRunning() { return m_is_active; }
	static const int& width;
	static const int& height;

	static SDL_Window* window;
	static SDL_Renderer* renderer;
#if defined(_SDL2_gfxPrimitives_h)
	static inline void drawFilledPolygon(const Sint16* vx, const Sint16* vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
		filledPolygonRGBA(renderer, vx, vy, n, r, g, b, a); }
#endif
private:
	DisplayWindow() {
		abort(); //there should not exist any objects of this class
	}

	static int m_width;
	static int m_height;
	static bool m_is_active;
};