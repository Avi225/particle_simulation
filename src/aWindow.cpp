#include <aWindow.hpp>

aWindow::aWindow(const char* title, int width, int height)
	: window(NULL), renderer(NULL)
{
	resolutionMultiplier = 2;
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	SDL_SetWindowResizable(window, SDL_TRUE);

	if (window == NULL)
		printf("window init failed.\n");

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width*resolutionMultiplier, height*resolutionMultiplier);

	SDL_SetRenderTarget(renderer, screenTexture);
	background = {30, 30, 30, 255};
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	font = TTF_OpenFont("res/font.ttf", 16*resolutionMultiplier);

}

SDL_Texture* aWindow::loadTexture(const char* filePath)
{
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, filePath);
	if (texture == NULL)
		printf("texture loading failed.\n");
	return texture;
}

int aWindow::getRefreshRate()
{
	int displayIndex = SDL_GetWindowDisplayIndex(window);

	SDL_DisplayMode mode;

	SDL_GetDisplayMode(displayIndex, 0, &mode);

	return mode.refresh_rate;
}

int aWindow::getResolutionMultiplier()
{
	return resolutionMultiplier;
}

void aWindow::cleanUp()
{
	SDL_DestroyWindow(window);
}

void aWindow::clear()
{
	SDL_RenderClear(renderer);
}

void aWindow::display()
{
	SDL_SetRenderTarget(renderer, NULL);
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	SDL_Rect rect = {0, 0, w, h};
	SDL_RenderCopy(renderer, screenTexture, NULL, &rect);
	SDL_RenderPresent(renderer);

	SDL_SetRenderTarget(renderer, screenTexture);
}

void aWindow::renderRect(SDL_Rect rect, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	rect = {rect.x*resolutionMultiplier, rect.y*resolutionMultiplier, rect.w*resolutionMultiplier, rect.h*resolutionMultiplier};
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
}


void aWindow::renderDisc(vector2d position, int radius, SDL_Color color)
{
	position *= resolutionMultiplier;
    radius *= resolutionMultiplier;

    int offsetx, offsety, d;

    offsetx = 0;
    offsety = radius;
    d = radius -1;

    SDL_SetRenderDrawColor(renderer, color.r,  color.g,  color.b,  color.a);

    while (offsety >= offsetx)
    {
        SDL_RenderDrawLine(renderer, position.x - offsety, position.y + offsetx, position.x + offsety, position.y + offsetx);
        SDL_RenderDrawLine(renderer, position.x - offsetx, position.y + offsety, position.x + offsetx, position.y + offsety);
        SDL_RenderDrawLine(renderer, position.x - offsetx, position.y - offsety, position.x + offsetx, position.y - offsety);
        SDL_RenderDrawLine(renderer, position.x - offsety, position.y - offsetx, position.x + offsety, position.y - offsetx);
      
        if (d >= 2*offsetx)
        {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety))
        {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else
        {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
	}

	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
}

void aWindow::getSize(int *w, int *h)
{
	SDL_GetWindowSize(window, w, h);
}

void aWindow::switchFullscreen()
{
	if(fullscreen)
	{
		SDL_SetWindowFullscreen(window, 0);
		fullscreen = false;

	}
	else
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		fullscreen = true;
	}
}

void aWindow::renderText(vector2d position, std::string text, int size, SDL_Color color)
{
	position *= resolutionMultiplier;

	SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect rect = {int(position.x), int(position.y), surface->w, surface->h};
	SDL_RenderCopy(renderer, texture, NULL, &rect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void aWindow::renderTexture(SDL_Texture* texture, SDL_Rect destination, double angle, SDL_Color color)
{
	//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(texture, color.a);
	destination = {destination.x * resolutionMultiplier, destination.y * resolutionMultiplier, destination.w * resolutionMultiplier, destination.h * resolutionMultiplier};
	SDL_RenderCopyEx(renderer, texture, NULL, &destination, -angle, NULL, SDL_FLIP_NONE);
}

void aWindow::renderPixel(vector2d position, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	
	position *= resolutionMultiplier;
	SDL_RenderDrawPoint(renderer, int(position.x), int(position.y));

	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);

}


void aWindow::updateSize(int nWidth, int nHeight)
{
	SDL_DestroyTexture(screenTexture);
	screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, nWidth*resolutionMultiplier, nHeight*resolutionMultiplier);
}

