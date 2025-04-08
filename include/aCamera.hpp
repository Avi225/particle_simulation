#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <cmath>
#include <unordered_map>

#include "aWindow.hpp"
#include "math.hpp"

class aWindow;

class aCamera
{
public:
	aCamera(int nWidth, int nHeight, aWindow* nWindow);
	void cleanUp();

	void moveCamera(vector2d movement);
	void zoomCamera(double nZoom);

	void updateZoom();
	void updatePosition();

	void setSize(int w, int h);
	void getSize(int* w, int* h);

	void loadTextures();

	vector2d screenToWorld(vector2d a);
	vector2d screenToCamera(vector2d a);
	vector2d worldToScreen(vector2d a);
	void worldToScreen(vector2d* a);

	void renderLine(vector2d a, vector2d b, double thickness, SDL_Color color, bool UI);

	void renderDisc(vector2d discPosition, double radius, SDL_Color color, bool UI);

	void renderRect(SDL_FRect rect, SDL_Color color, bool UI);

	void renderText(vector2d position, double height, std::string alignment, std::string text, SDL_Color color, bool UI);

private:
	int width;
	int height;
	aWindow* window;

	vector2d position;
	vector2d targetPosition;
	double positionSmoothness;

	int spriteSize;

	std::unordered_map<std::string, SDL_Texture*> textures;

	double zoomMin;
	double zoomMax;
	double zoomScale;
	double zoom;
	double targetZoom;
	double zoomSmoothness;
};