#include "aCamera.hpp"


// aCamera class constructor
aCamera::aCamera(int nWidth, int nHeight, aWindow* nWindow)
: width(nWidth), height(nHeight), window(nWindow)
{
	position = vector2d(0, 0);
	zoom = 1;
	zoomMin = 0;
	zoomMax = 200;
	zoomScale = 5;
	positionSmoothness = 8;
	zoomSmoothness = 8;
	targetZoom = 1;
	targetPosition = vector2d(0, 0);

	textures["basic"] = window -> loadTexture("res/basic.png"); // Used for rendering with downscaling to antialias the final image
	textures["disc"] = window -> loadTexture("res/disc.png");
}

// Adjust the target position
void aCamera::moveCamera(vector2d movement)
{
	movement /= (zoom/2);
	targetPosition += movement;
}

// Adjust the target zoom level
void aCamera::zoomCamera(double nZoom)
{
	targetZoom += nZoom*targetZoom;
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
	position += (targetPosition - position) / positionSmoothness;
}

// Set the size
void aCamera::setSize(int w, int h)
{
	width = w;
	height = h;
}

// Convert screen coordinates to world coordinates
vector2d aCamera::screenToWorld(vector2d a)
{
	a.x -= width / 2;
	a.y -= height / 2;

	a /= (zoomScale * zoom);
	a -= position;

	return a;
}

// Convert world coordinates to screen coordinates
void aCamera::worldToScreen(vector2d* a)
{
	a -> x += position.x;
	a -> y += position.y;

	a -> x *= (zoomScale * zoom);
	a -> y *= (zoomScale * zoom);

	a -> x += width / 2;
	a -> y += height / 2;
}


// Render a line on the screen
void aCamera::renderLine(vector2d a, vector2d b, double thickness, SDL_Color color, bool UI)
{
	SDL_Rect rect;
	if(!UI)
	{
		worldToScreen(&a);
		worldToScreen(&b);

		rect = 
		{
			int((a.x+b.x)/2-thickness * zoomScale * zoom/2),
			int((a.y+b.y)/2-a.distance(b)/2),
			int(clamp(thickness * zoomScale * zoom, 1)),
			int(a.distance(b))
		};
	}else
	{
		rect = 
		{
			int((a.x+b.x)/2-thickness),
			int((a.y+b.y)/2-a.distance(b)/2),
			int(thickness),
			int(a.distance(b))
		};
	}
	vector2d rotation = a.getVector(b);
	rotation.normalize(1);
	double angle = rotation.getAngle();
	window -> renderTexture(textures["basic"], rect, angle, color); 
}

// Render a colored disc (circle) on the screen
void aCamera::renderDisc(vector2d discPosition, double radius, SDL_Color color, bool UI)
{
	if(!UI)
	{
		radius *= zoomScale * zoom;
		worldToScreen(&discPosition);
		if(radius < 0.5)
		{
			window -> renderPixel(discPosition, color);
			return;
		}

		discPosition -= radius;

		SDL_Rect rect = 
		{
			int(discPosition.x),
			int(discPosition.y),
			int(2*radius),
			int(2*radius)
		};

		window -> renderTexture(textures["disc"], rect, 0, color);

	}else
	{
		window -> renderDisc(discPosition, radius, color);
	}

}

// Render a colored rectangle on the screen
void aCamera::renderRect(SDL_Rect rect, SDL_Color color, bool UI)
{

	if(!UI)
	{
		rect.x += position.x;
		rect.y += position.y;

		rect.x *= (zoomScale * zoom);
		rect.y *= (zoomScale * zoom);

		rect.x += width / 2;
		rect.y += height / 2;

		rect.w *= (zoomScale * zoom);
		rect.h *= (zoomScale * zoom);

	}
	window -> renderRect(rect, color);
}

void aCamera::renderRect(vector2d position, vector2d size, SDL_Color color, bool UI)
{

	if(!UI)
	{
		worldToScreen(&position);

		size.x *= (zoomScale * zoom);
		size.y *= (zoomScale * zoom);
	}
	
	SDL_Rect rect =
	{
		int(position.x),
		int(position.y),
		int(size.x),
		int(size.y)
	};

	window -> renderRect(rect, color);
}

// Render text on the screen
void aCamera::renderText(vector2d textPosition, int size, std::string text, SDL_Color color, bool UI)
{
	if(!UI)
		worldToScreen(&textPosition);

	window -> renderText(textPosition, text, size, color);
}