#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <cstdio>

#include "math.hpp"

class aWindow
{
public:
	aWindow(const char* title, int width, int height);

	SDL_Texture* loadTexture(const char* filePath);

	int getRefreshRate();

	int getResolutionMultiplier();

	void cleanUp();

	void clear();

	void display();

	void renderRect(SDL_Rect rect, SDL_Color color);

	void renderDisc(vector2f position, int radius, SDL_Color color);

	void getSize(int* w, int* h);

	void switchFullscreen();

	void renderText(vector2f position, std::string text, int size, SDL_Color color);

	void renderTexture(SDL_Texture* texture, SDL_Rect destination, double angle, SDL_Color color);

	void updateSize(int nWidth, int nHeight);

private:
	SDL_Window* window;

	SDL_Renderer* renderer;

	SDL_Texture* screenTexture;

	SDL_Color background;

	int resolutionMultiplier;

	bool fullscreen;

	TTF_Font* font;
};