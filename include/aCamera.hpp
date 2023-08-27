#pragma once

#include <cmath>

#include "aWindow.hpp"
#include "math.hpp"


class aCamera
{
public:
	aCamera(int nWidth, int nHeight, aWindow* nWindow);

	void moveCamera(vector2f movement);
	void zoomCamera(float nZoom);

	void updateZoom();
	void updatePosition();

	void setSize(int w, int h);

	vector2f screenToWorld(aWindow *window, vector2f a);

	void renderLine(vector2f a, vector2f b, float thickness, SDL_Color color);

	void renderDisc(vector2f discPosition, float radius, SDL_Color color);

	void renderRect(SDL_Rect rect, SDL_Color color, bool UI);

	void renderText(vector2f textPosition, int size, std::string text, SDL_Color color, bool UI);

private:
	int width;
	int height;
	aWindow* window;

	vector2f position;
	vector2f targetPosition;
	float positionSmoothness;

	SDL_Texture* basic;

	float zoomMin;
	float zoomMax;
	float zoomScale;
	float zoom;
	float targetZoom;
	float zoomSmoothness;
};