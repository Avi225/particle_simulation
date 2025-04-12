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
	targetZoom = 0.05;
	targetPosition = vector2d(0, 0);
	spriteSize = 64;
	log::info("aCamera::aCamera - Constructor finished successfully.");
}

void aCamera::cleanUp()
{
	for (auto const& [key, texture] : textures)
    {
        if (texture)
        {
		    SDL_DestroyTexture(texture);
		}
    }
    textures.clear();
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

// Get the size
void aCamera::getSize(int* w, int* h)
{
	*w = width;
	*h = height;
}

void aCamera::loadTextures()
{
	textures["basic"] = window -> loadTexture("res/basic.png");
	textures["disc"] = window -> loadTexture("res/disc.png");
	
	for (const auto& pair : textures) {
        if(!SDL_SetTextureScaleMode(pair.second, SDL_SCALEMODE_LINEAR))
        {
			log::error("aCamera::loadTextures - Set texture scale mode error: {}", SDL_GetError());
		}
		if(!SDL_SetTextureBlendMode(pair.second, SDL_BLENDMODE_BLEND))
        {
			log::error("aCamera::loadTextures - Set texture blend mode error: {}", SDL_GetError());
		}
    }
}

vector2d aCamera::screenToWorld(vector2d a)
{
	a.x -= width / 2;
	a.y -= height / 2;

	a /= (zoomScale * zoom);
	a -= position;

	return a;
}

vector2d aCamera::screenToCamera(vector2d a)
{
	a.x -= width / 2;
	a.y -= height / 2;
	a /= (zoomScale * zoom);
	return a;
}

void aCamera::worldToScreen(vector2d* a)
{
	a -> x += position.x;
	a -> y += position.y;

	a -> x *= (zoomScale * zoom);
	a -> y *= (zoomScale * zoom);

	a -> x += width / 2;
	a -> y += height / 2;
}

vector2d aCamera::worldToScreen(vector2d a)
{
	a.x += position.x;
	a.y += position.y;

	a.x *= (zoomScale * zoom);
	a.y *= (zoomScale * zoom);

	a.x += width / 2;
	a.y += height / 2;
	return a;
}

// Render a line on the screen
void aCamera::renderLine(vector2d a, vector2d b, double thickness, SDL_Color color, bool UI)
{
    SDL_FRect rect = {0.0f, 0.0f, 0.0f, 0.0f};
    vector2d screenPosA = a;
    vector2d screenPosB = b;
    float screenDistance = 0.0f;
    float screenThickness = 0.0f;

    if (!UI)
    {
		screenPosA = worldToScreen(a);
		screenPosB = worldToScreen(b);

        screenThickness = static_cast<float>(clamp(thickness * zoomScale * zoom, 1.0));
    }
    else
    {
        screenThickness = static_cast<float>(clamp(thickness, 1.0));
    }

    screenDistance = static_cast<float>(screenPosA.distance(screenPosB));

    if (screenDistance <= 0.0f || screenThickness <= 0.0f)
    {
        return;
    }
    rect =
    {
        static_cast<float>((screenPosA.x + screenPosB.x) / 2.0 - screenThickness / 2.0),
        static_cast<float>((screenPosA.y + screenPosB.y) / 2.0 - screenDistance / 2.0),
        screenThickness,
        screenDistance
    };

    vector2d rotationVector = screenPosA.getVector(screenPosB);

    double angle = rotationVector.getAngle();

    auto it = textures.find("basic");
    if (it != textures.end() && it->second != nullptr)
    {
        window->renderTexture(it->second, nullptr, &rect, angle, color);
    }
    else
    {
        log::error("aCamera::renderLine - 'basic' texture not found.");
    }
}

// Render a colored disc (circle) on the screen
void aCamera::renderDisc(vector2d discPosition, double radius, SDL_Color color, bool UI)
{
	if(!UI)
	{
		radius *= zoomScale * zoom;
		worldToScreen(&discPosition);

		discPosition -= radius;

		SDL_FRect rect = 
		{
			float(discPosition.x),
			float(discPosition.y),
			float(2*radius),
			float(2*radius)
		};

		window -> renderTexture(textures["disc"], nullptr, &rect, 0, color);

	}else
	{
		window -> renderDisc(discPosition, int(radius), color);
	}

}

// Render a colored rectangle on the screen
void aCamera::renderRect(SDL_FRect rect, SDL_Color color, bool UI)
{
	if(UI)
	{
		window -> renderRect(rect, color);
	}
	else
	{
		rect.x += float(position.x);
		rect.y += float(position.y);

		rect.x *= float(zoomScale * zoom);
		rect.y *= float(zoomScale * zoom);

		rect.x += float(width / 2);
		rect.y += float(height / 2);

		rect.w *= float(zoomScale * zoom);
		rect.h *= float(zoomScale * zoom);

		window -> renderRect(rect, color);
	}
}

// Render text on the screen
void aCamera::renderText(vector2d position, double height, std::string alignment, std::string text, SDL_Color color, bool UI)
{
	if(UI)
	{
		window -> renderText(position, height, alignment, text, color);
	}
	else
	{
		worldToScreen(&position);
		height *= zoomScale * zoom;

		window -> renderText(position, height, alignment, text, color);
	}
};