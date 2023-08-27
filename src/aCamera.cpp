#include "aCamera.hpp"

aCamera::aCamera(int nWidth, int nHeight, aWindow* nWindow)
: width(nWidth), height(nHeight), window(nWindow)
{
	position = vector2f(0, 0);
	zoom = 1;
	zoomMin = 1;
	zoomMax = 200;
	zoomScale = 5;
	positionSmoothness = 8;
	zoomSmoothness = 8;
	targetZoom = 1;
	targetPosition = vector2f(0, 0);
	basic = window -> loadTexture("res/basic.png");
}

void aCamera::moveCamera(vector2f movement)
{
	targetPosition.x += movement.x/(zoom/2);
	targetPosition.y += movement.y/(zoom/2);
}

void aCamera::zoomCamera(float nZoom)
{
	targetZoom += nZoom;
	targetZoom = clamp(targetZoom, zoomMin, zoomMax);
}


void aCamera::updateZoom()
{
	zoom += (targetZoom - zoom)/zoomSmoothness;
	zoom = clamp(zoom, zoomMin, zoomMax);
}

void aCamera::updatePosition()
{
	position.x += (targetPosition.x - position.x) / positionSmoothness;
	position.y += (targetPosition.y - position.y) / positionSmoothness;
}

void aCamera::setSize(int w, int h)
{
	width = w;
	height = h;
}

vector2f aCamera::screenToWorld(aWindow *window, vector2f a)
{
	int w, h;
	window -> getSize(&w, &h);

	a.x -= width / 2;
	a.y -= height / 2;

	a.x /= (zoomScale * zoom);
	a.y /= (zoomScale * zoom);

	a.x -= position.x;
	a.y -= position.y;

	return a;
}

void aCamera::renderLine(vector2f a, vector2f b, float thickness, SDL_Color color)
{
	a.x += position.x;
	a.y += position.y;

	b.x += position.x;
	b.y += position.y;

	a.x *= zoomScale * zoom;
	a.y *= zoomScale * zoom;

	b.x *= zoomScale * zoom;
	b.y *= zoomScale * zoom;

	a.x += width / 2;
	a.y += height / 2;

	b.x += width / 2;
	b.y += height / 2;

	SDL_Rect rect = 
	{
		int((a.x+b.x)/2-thickness * zoomScale * zoom/2),
		int((a.y+b.y)/2-a.distance(b)/2),
		int(clamp(thickness * zoomScale * zoom, 1)),
		int(a.distance(b))
	};
	vector2f rotation = a.getVector(b);
	rotation.normalize(1);
	double angle = rotation.getAngle();
	window -> renderTexture(basic, rect, angle, color); 
}

void aCamera::renderDisc(vector2f discPosition, float radius, SDL_Color color)
{
	radius *= zoomScale * zoom;

	discPosition.x += position.x;
	discPosition.y += position.y;

	discPosition.x *= zoomScale*zoom;
	discPosition.y *= zoomScale*zoom;

	discPosition.x += width/2;
	discPosition.y += height/2;

	window -> renderDisc(discPosition, radius, color);
}

void aCamera::renderRect(SDL_Rect rect, SDL_Color color, bool UI)
{
	if(UI)
		window -> renderRect(rect, color);
	else
	{
		rect.x += position.x;
		rect.y += position.y;

		rect.x += width / 2;
		rect.y += height / 2;

		window -> renderRect(rect, color);
	}
}


void aCamera::renderText(vector2f textPosition, int size, std::string text, SDL_Color color, bool UI)
{
	if(UI)
		window -> renderText(textPosition, text, size, color);
	else
	{
		textPosition.x += position.x;
		textPosition.y += position.y;

		textPosition.x *= zoomScale*zoom;
		textPosition.y *= zoomScale*zoom;

		textPosition.x += width/2;
		textPosition.y += height/2;

		window -> renderText(textPosition, text, size, color);
	}
}