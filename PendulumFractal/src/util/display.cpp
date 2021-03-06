#include "display.h"
#include <string>
#include <iostream>


SDL_Window* DisplayWindow::window = nullptr;
SDL_Renderer* DisplayWindow::renderer = nullptr;
bool DisplayWindow::m_is_active = false;
int  DisplayWindow::m_width = 0;
int  DisplayWindow::m_height = 0;
const int&  DisplayWindow::width = DisplayWindow::m_width;
const int&  DisplayWindow::height = DisplayWindow::m_height;



void DisplayWindow::init(int t_width, int t_height) {
	if (m_is_active == false) {
		//log__vv("DisplayWindow::init");
		SDL_Init(SDL_INIT_VIDEO);
		m_width = t_width;
		m_height = t_height;
		m_is_active = true;

		// Create an application window with the following settings:
		window = SDL_CreateWindow(
			"An SDL2 window",                  // window title
			SDL_WINDOWPOS_UNDEFINED,           // initial x position
			SDL_WINDOWPOS_UNDEFINED,           // initial y position
			m_width,                           // width, in pixels
			m_height,                          // height, in pixels
			SDL_WINDOW_OPENGL                  // flags - see below
		);

		// Create renderer
		renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		// Check that the window was successfully created
		if (window == NULL)
		{
			printf("Could not create window: %s\n", SDL_GetError());
		}
	}
	else {
		abort(); //double initialization not allowed
	}
}

void DisplayWindow::deinit() {
	if (m_is_active == true) {
		m_is_active = false;
		//log__vv("DisplayWindow::deinit");
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		//log__vv("window closed");
		SDL_Quit();
	}
}

void DisplayWindow::flip() {
	SDL_RenderPresent(renderer);
}

void DisplayWindow::update() {
	SDL_Event sevent;
	while (SDL_PollEvent(&sevent)) {
		switch (sevent.type) {
			case SDL_KEYDOWN: {
				if (sevent.key.keysym.sym == SDLK_ESCAPE) {
					deinit();
					return;
				}
				break;
			}
			case SDL_QUIT: {
				deinit();
				return;
			}
		}
	}
}

void DisplayWindow::drawRectangle(int x, int y, int w, int h, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_RenderFillRect(renderer, &rect);
}

void DisplayWindow::fill(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
	SDL_RenderClear(renderer);
}

Uint32 DisplayWindow::getPixel32(SDL_Surface* surface, int x, int y) {
	if (surface) {
		//Convert the pixels to 32 bit
		Uint32* pixels = (Uint32*)surface->pixels;

		//Get the requested pixel
		return pixels[(y * surface->w) + x];
	}
	return 0;
}

SDL_Texture* DisplayWindow::loadTexture(std::string path) {
	return IMG_LoadTexture(renderer, path.c_str());
}

void DisplayWindow::saveTextureToFile(const char* file_name, SDL_Texture* texture) {
	SDL_Texture* target = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, texture);
	int width, height;
	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
	SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
	IMG_SavePNG(surface, file_name);
	SDL_FreeSurface(surface);
	SDL_SetRenderTarget(renderer, target);
}

SDL_Texture* DisplayWindow::createTextureRGBA(int width, int height) {
	return SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		width,
		height);
}

void DisplayWindow::copyTextureToRenderTarget(SDL_Texture* texture, SDL_Rect* src_rect, SDL_Rect* dest_rect) {
	SDL_RenderCopy(renderer, texture, src_rect, dest_rect);
}

void DisplayWindow::copyTextureToRenderTarget(SDL_Texture* texture, SDL_Rect* src_rect, SDL_Rect* dest_rect, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_RenderCopyEx(renderer, texture, src_rect, dest_rect, angle, center, flip);
}

void DisplayWindow::destroyTexture(SDL_Texture* texture) {
	SDL_DestroyTexture(texture);
}

SDL_Surface* DisplayWindow::loadSurface(std::string path) {
	return IMG_Load(path.c_str());
}

void DisplayWindow::destroySurface(SDL_Surface* surface) {
	SDL_FreeSurface(surface);
}

int DisplayWindow::getMouseXPos() {
	int x;
	SDL_GetMouseState(&x, nullptr);
	return x;
}

int DisplayWindow::getMouseYPos() {
	int y;
	SDL_GetMouseState(nullptr, &y);
	return y;
}

const Uint8* DisplayWindow::getKeyboardState() {
	return SDL_GetKeyboardState(NULL);
	//example:
	//while (DisplayWindow::isRunning()) {
	//	DisplayWindow::update();
	//	const Uint8* state = DisplayWindow::getKeyboardState();
	//	std::cout << (bool)state[SDL_SCANCODE_C] << std::endl;
	//}
}