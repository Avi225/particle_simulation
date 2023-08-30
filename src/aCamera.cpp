#include "aCamera.hpp"


// aCamera class constructor
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
	basic = window -> loadTexture("res/basic.png"); // Used for rendering with downscaling to antialias the final image
}

// Adjust the target position
void aCamera::moveCamera(vector2f movement)
{
	movement.x /= (zoom/2);
	movement.y /= (zoom/2);
	targetPosition += movement;
}

// Adjust the target zoom level
void aCamera::zoomCamera(float nZoom)
{
	targetZoom += nZoom;
	targetZoom = clamp(targetZoom, zoomMin, zoomMax);
}

// Smoothly update the current zoom
void aCamera::updateZoom()
{
	zoom += (targetZoom - zoom)/zoomSmoothness;
	zoom = clamp(zoom, zoomMin, zoomMax);
}

// Smoothly update the current position
void aCamera::updatePosition()
{
	position.x += (targetPosition.x - position.x) / positionSmoothness;
	position.y += (targetPosition.y - position.y) / positionSmoothness;
}

// Set the size
void aCamera::setSize(int w, int h)
{
	width = w;
	height = h;
}

// Convert screen coordinates to world coordinates
vector2f aCamera::screenToWorld(vector2f a)
{
	a.x -= width / 2;
	a.y -= height / 2;

	a.x /= (zoomScale * zoom);
	a.y /= (zoomScale * zoom);

	a.x -= position.x;
	a.y -= position.y;

	return a;
}

// Convert world coordinates to screen coordinates
void aCamera::worldToScreen(vector2f* a)
{
	a -> x += position.x;
	a -> y += position.y;

	a -> x *= (zoomScale * zoom);
	a -> y *= (zoomScale * zoom);

	a -> x += width / 2;
	a -> y += height / 2;
}


// Render a line on the screen
void aCamera::renderLine(vector2f a, vector2f b, float thickness, SDL_Color color)
{
	worldToScreen(&a);
	worldToScreen(&b);

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

// Render a colored disc (circle) on the screen
void aCamera::renderDisc(vector2f discPosition, float radius, SDL_Color color)
{
	radius *= zoomScale * zoom;

	worldToScreen(&discPosition);

	window -> renderDisc(discPosition, radius, color);
}

// Render a colored rectangle on the screen
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

// Render text on the screen
void aCamera::renderText(vector2f textPosition, int size, std::string text, SDL_Color color, bool UI)
{
	if(UI)
		window -> renderText(textPosition, text, size, color);
	else
	{
		worldToScreen(&textPosition);

		window -> renderText(textPosition, text, size, color);
	}
}