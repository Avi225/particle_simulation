#pragma once

#include <cmath>
#include <unordered_map>

#include "aWindow.hpp"
#include "math.hpp"


class aCamera
{
public:
	aCamera(int nWidth, int nHeight, aWindow* nWindow);

	void moveCamera(vector2d movement);
	void zoomCamera(double nZoom);

	void updateZoom();
	void updatePosition();

	void setSize(int w, int h);

	vector2d screenToWorld(vector2d a);
	vector2d worldToScreen(vector2d a);
	void worldToScreen(vector2d* a);

	void renderLine(vector2d a, vector2d b, double thickness, SDL_Color color, bool UI);

	void renderDisc(vector2d discPosition, double radius, SDL_Color color, bool UI);

	void renderRect(SDL_Rect rect, SDL_Color color, bool UI);
	void renderRect(vector2d position, vector2d size, SDL_Color color, bool UI);

	void renderText(vector2d textPosition, int size, std::string text, SDL_Color color, bool UI);

private:
	int width;
	int height;
	aWindow* window;

	vector2d position;
	vector2d targetPosition;
	double positionSmoothness;

	std::unordered_map<std::string, SDL_Texture*> textures;

	double zoomMin;
	double zoomMax;
	double zoomScale;
	double zoom;
	double targetZoom;
	double zoomSmoothness;
};