#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <unordered_map>

#include "log.hpp"
#include "math.hpp"

class aWindow
{
public:
	aWindow(const char* title, int width, int height);

	SDL_Texture* loadTexture(const char* filePath);

	int getRefreshRate();

	double getResolutionMultiplier();

	void cleanUp();

	void clear();

	void display();

	void renderRect(SDL_FRect rect, SDL_Color color);

	void renderDisc(vector2d position, int radius, SDL_Color color);

	void getSize(int* w, int* h);

	SDL_Texture* getScreenTexture();

	void switchFullscreen();

	void renderText(vector2d position, double height, std::string alignment, std::string text, SDL_Color color);

	void renderTexture(SDL_Texture* texture, SDL_FRect* source, SDL_FRect* destination, double angle, SDL_Color color);

	void updateSize(int nWidth, int nHeight);

private:
	SDL_Window* window;

	SDL_Renderer* renderer;

	SDL_Texture* screenTexture;

	TTF_TextEngine* textEngine;

	SDL_Color background;

	double resolutionMultiplier;

	bool fullscreen;

	std::unordered_map<int, TTF_Font*> fonts;

	int windowWidth, windowHeight;
};