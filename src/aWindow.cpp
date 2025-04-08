#include <SDL3/SDL.h>

#include "aWindow.hpp"

aWindow::aWindow(const char* title, int width, int height)
	: window(NULL), renderer(NULL)
{
	resolutionMultiplier = 2;
	window = SDL_CreateWindow(title, width, height, 0);
	if (window == NULL)
	{
        fprintf(stderr, "FATAL ERROR: SDL_CreateWindow failed: %s\n", SDL_GetError());
        throw std::runtime_error("Window creation failed");
    }
	if(!SDL_SetWindowResizable(window, true))
	{
		fprintf(stderr, "aWindow::aWindow - Set window resizable error: %s\n", SDL_GetError()); fflush(stderr);
	}

	renderer = SDL_CreateRenderer(window, NULL);
	if (renderer == NULL)
	{
        fprintf(stderr, "FATAL ERROR: SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        throw std::runtime_error("Renderer creation failed");
    }
	screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width*resolutionMultiplier, height*resolutionMultiplier);
	if (screenTexture == NULL)
	{
        fprintf(stderr, "FATAL ERROR: SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        throw std::runtime_error("Screen texture creation failed");
    }
	SDL_SetRenderTarget(renderer, screenTexture);
	background = {30, 30, 30, 255};
	fullscreen = false;
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	textEngine = TTF_CreateSurfaceTextEngine();
    if (textEngine == NULL)
    {
        fprintf(stderr, "FATAL ERROR: TTF_CreateSurfaceTextEngine failed: %s\n", SDL_GetError());
        SDL_DestroyTexture(screenTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window); 
        throw std::runtime_error("Text engine creation failed");
    }

    for (int size = 8; size <= 128; size += 2)
    {
        TTF_Font* tempFont = TTF_OpenFont("res/font.ttf", float(size));
        if (tempFont == NULL) {
            fprintf(stderr, "aWindow::aWindow - Open font error: %s\n", SDL_GetError()); fflush(stderr);
            continue;
        } 
        else 
        {
            fonts[size] = tempFont;
        }
    }
    fprintf(stderr, "aWindow::aWindow - Constructor finished successfully.\n"); fflush(stderr);
}

SDL_Texture* aWindow::loadTexture(const char* filePath)
{
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, filePath);
	if (texture == NULL)
	{
		fprintf(stderr, "aWindow::loadTexture - Load texture error: %s\n", SDL_GetError()); fflush(stderr);
	}
	return texture;
}

int aWindow::getRefreshRate()
{
    int displayIndex = SDL_GetDisplayForWindow(window);
    if(displayIndex == 0)
    {
    	fprintf(stderr, "aWindow::getRefreshRate - Get display for window error: %s\n", SDL_GetError()); fflush(stderr);
        return 0;
    }

	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(displayIndex);
	if(mode == NULL)
    {
    	fprintf(stderr, "aWindow::getRefreshRate - Get current display mode error: %s\n", SDL_GetError()); fflush(stderr);
        return 0;
    }
	return int(mode -> refresh_rate);
}

int aWindow::getResolutionMultiplier()
{
	return resolutionMultiplier;
}

void aWindow::cleanUp()
{
	// Destroy fonts
    for (auto const& [size, font] : fonts) {
        if (font) {
            TTF_CloseFont(font);
        }
    }
    fonts.clear();

    // Destroy screen texture
    if (screenTexture) {
        SDL_DestroyTexture(screenTexture);
        screenTexture = nullptr;
    }
    // Destroy Text Engine
    if(textEngine) {
    	 TTF_DestroySurfaceTextEngine(textEngine);
         textEngine = nullptr;
    }
    // Destroy Renderer
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    // Destroy Window
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void aWindow::clear()
{
	if(!SDL_RenderClear(renderer))
	{
    	fprintf(stderr, "aWindow::clear - Render clear error: %s\n", SDL_GetError()); fflush(stderr);
    }
}

void aWindow::display()
{
    if(!SDL_SetRenderTarget(renderer, NULL))
    {
    	fprintf(stderr, "aWindow::display - Set render target error: %s\n", SDL_GetError()); fflush(stderr);
    }
    int w, h;
    if(!SDL_GetWindowSize(window, &w, &h))
    {
    	fprintf(stderr, "aWindow::display - Get window size error: %s\n", SDL_GetError()); fflush(stderr);
    }
    SDL_FRect rect = {0, 0, float(w), float(h)};
    if(!SDL_RenderTexture(renderer, screenTexture, NULL, &rect))
    {
    	fprintf(stderr, "aWindow::display - Render texture error: %s\n", SDL_GetError()); fflush(stderr);
    }
    if(!SDL_RenderPresent(renderer))
    {
    	fprintf(stderr, "aWindow::display - Render present error: %s\n", SDL_GetError()); fflush(stderr);
    }
    if(!SDL_SetRenderTarget(renderer, screenTexture))
    {
    	fprintf(stderr, "aWindow::display - Set render target error: %s\n", SDL_GetError()); fflush(stderr);
    }
}

void aWindow::renderRect(SDL_FRect rect, SDL_Color color)
{
    rect = {rect.x * resolutionMultiplier,
            rect.y * resolutionMultiplier,
            rect.w * resolutionMultiplier,
            rect.h * resolutionMultiplier};
    int ww, wh;
    getSize(&ww, &wh);
    ww *= resolutionMultiplier;
    wh *= resolutionMultiplier;

    if (rect.x > ww || rect.y > wh || rect.x + rect.w < 0 || rect.y + rect.h < 0)
    {
        return;
    }

    if(!SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a))
    {
    	fprintf(stderr, "aWindow::renderRect - Set render draw color error: %s\n", SDL_GetError()); fflush(stderr);
    }
    if(!SDL_RenderFillRect(renderer, &rect))
    {
    	fprintf(stderr, "aWindow::renderRect - Render fill rect error: %s\n", SDL_GetError()); fflush(stderr);
    }
    if(!SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a))
    {
    	fprintf(stderr, "aWindow::renderRect - Set render draw color error: %s\n", SDL_GetError()); fflush(stderr);
    }
}


void aWindow::renderDisc(vector2d position, int radius, SDL_Color color)
{
    position *= resolutionMultiplier;
    radius *= resolutionMultiplier;

    int offsetx, offsety, d;

    offsetx = 0;
    offsety = radius;
    d = radius -1;

    if(!SDL_SetRenderDrawColor(renderer, color.r,  color.g,  color.b,  color.a))
    {
    	fprintf(stderr, "aWindow::renderDisc - Set render draw color error: %s\n", SDL_GetError()); fflush(stderr);
    }

    while (offsety >= offsetx)
    {
        if(!SDL_RenderLine(renderer, float(position.x - offsety), float(position.y + offsetx), float(position.x + offsety), float(position.y + offsetx)))
        {
        	fprintf(stderr, "aWindow::renderDisc - Render line error: %s\n", SDL_GetError()); fflush(stderr);
        }
        if(!SDL_RenderLine(renderer, float(position.x - offsetx), float(position.y + offsety), float(position.x + offsetx), float(position.y + offsety)))
        {
        	fprintf(stderr, "aWindow::renderDisc - Render line error: %s\n", SDL_GetError()); fflush(stderr);
        }
        if(!SDL_RenderLine(renderer, float(position.x - offsetx), float(position.y - offsety), float(position.x + offsetx), float(position.y - offsety)))
        {
        	fprintf(stderr, "aWindow::renderDisc - Render line error: %s\n", SDL_GetError()); fflush(stderr);
        }
        if(!SDL_RenderLine(renderer, float(position.x - offsety), float(position.y - offsetx), float(position.x + offsety), float(position.y - offsetx)))
        {
        	fprintf(stderr, "aWindow::renderDisc - Render line error: %s\n", SDL_GetError()); fflush(stderr);
        }
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

    if(!SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a))
    {
    	fprintf(stderr, "aWindow::renderDisc - Set render draw color error: %s\n", SDL_GetError()); fflush(stderr);
    }
}

void aWindow::getSize(int *w, int *h)
{
    if(!SDL_GetWindowSize(window, w, h))
    {
    	fprintf(stderr, "aWindow::getSize - Window size fetch error: %s\n", SDL_GetError()); fflush(stderr);
    }
}

void aWindow::switchFullscreen()
{
    if (fullscreen)
    {
        if(SDL_SetWindowFullscreen(window, false))
    	{
    		fullscreen = false;
		}else
		{
			fprintf(stderr, "aWindow::switchFullscreen - Set window fullscreen error: %s\n", SDL_GetError()); fflush(stderr);
		}
    }
    else
    {
        if(SDL_SetWindowFullscreen(window, true))
    	{
    		fullscreen = true;
		}else
		{
			fprintf(stderr, "aWindow::switchFullscreen - Set window fullscreen error: %s\n", SDL_GetError()); fflush(stderr);
		}
    }
}

void aWindow::renderText(vector2d position, double height, std::string alignment, std::string text, SDL_Color color)
{
	position *= resolutionMultiplier;
	height *= resolutionMultiplier;

	SDL_FRect rect = {float(position.x), float(position.y), 0.0, float(height*(std::count(text.begin(), text.end(), '\n' + 1)))};

	if(alignment=="c")
		rect.y -= (rect.h / 2);
	else if (alignment=="e")
		rect.y -= (rect.h / 2);	
	else if (alignment=="se")
		rect.y -= rect.h;
	else if (alignment=="s")
		rect.y -= rect.h;
	else if (alignment=="sw")
		rect.y -= rect.h;
	else if (alignment=="w")
		rect.y -= (rect.h / 2);
	rect.h = float(height);
	std::istringstream stream(text);
    std::string line;
 	for (int i = 0; std::getline(stream, line); ++i)
    {
    	TTF_Font* bestFont = nullptr;
		int bestSize = std::numeric_limits<int>::max();

	    for (const auto& [size, font] : fonts)
	    {
	        if (size >= rect.h && size < bestSize)
	        {
	            bestSize = size;
	            bestFont = font;
	        }
	    }

	    if (!bestFont)
	    {
	        int maxSize = 0;
	        for (const auto& [size, font] : fonts)
	        {
	            if (size > maxSize)
	            {
	                maxSize = size;
	                bestFont = font;
	            }
	        }
	    }

    	int w, h;
    	if(!TTF_GetStringSize(bestFont, line.c_str(), line.size(), &w, &h))
    	{
    		fprintf(stderr, "aWindow::renderText - Get string size error: %s\n", SDL_GetError()); fflush(stderr);
    	}
		rect.y += float(height*bool(i));
		rect.w = float(w);
		rect.w *= float(height/h);
		rect.x = float(position.x);

		if(alignment=="c")
			rect.x -= (rect.w / 2);
		else if (alignment=="n")
			rect.x -= (rect.w / 2);
		else if (alignment=="ne")
			rect.x -= rect.w;
		else if (alignment=="e")
			rect.x -= rect.w;
		else if (alignment=="se")
			rect.x -= rect.w;
		else if (alignment=="s")
			rect.x -= (rect.w / 2);     

		int ww, wh;
		getSize(&ww, &wh);
		ww *= resolutionMultiplier;
		wh *= resolutionMultiplier;
		
		if(rect.x > ww || rect.y > wh || rect.x+rect.w < 0 || rect.y+rect.h < 0)
		{
			continue;
		}

	    TTF_Text* text = TTF_CreateText(textEngine, bestFont, line.c_str(), line.size());
	    if(text==NULL)
	    {
	    	fprintf(stderr, "aWindow::renderText - Create text error: %s\n", SDL_GetError()); fflush(stderr);
	    }

		SDL_Surface *surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
		if(surface==NULL)
		{
	    	fprintf(stderr, "aWindow::renderText - Create surface error: %s\n", SDL_GetError()); fflush(stderr);
		}

		if(!TTF_DrawSurfaceText(text, 0, 0, surface))
		{
			fprintf(stderr, "aWindow::renderText - Draw surface text error: %s\n", SDL_GetError()); fflush(stderr);
		}
		
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
		if(texture==NULL)
		{
	    	fprintf(stderr, "aWindow::renderText - Create texture from surface error: %s\n", SDL_GetError()); fflush(stderr);
		}

		if(!SDL_RenderTexture(renderer, texture, NULL, &rect))
		{
			fprintf(stderr, "aWindow::renderText - Render texture error: %s\n", SDL_GetError()); fflush(stderr);
		}

		TTF_DestroyText(text);
		SDL_DestroySurface(surface);
		SDL_DestroyTexture(texture);
	}
}

void aWindow::renderTexture(SDL_Texture* texture, SDL_FRect* source, SDL_FRect* destination, double angle, SDL_Color color)
{
	SDL_FRect dst = *destination;

	dst.x *= resolutionMultiplier;
	dst.y *= resolutionMultiplier;
	dst.w *= resolutionMultiplier;
	dst.h *= resolutionMultiplier;

	if(dst.w < 1)
		dst.w = 1;
	if(dst.h < 1)
		dst.h = 1;
    int ww, wh;
    getSize(&ww, &wh);
    ww *= resolutionMultiplier;
    wh *= resolutionMultiplier;

    if(!SDL_SetTextureColorMod(texture, color.r, color.g, color.b))
    {
    	fprintf(stderr, "aWindow::renderTexture - Set texture color mod error: %s\n", SDL_GetError()); fflush(stderr);
    }
    if(!SDL_SetTextureAlphaMod(texture, color.a))
    {
    	fprintf(stderr, "aWindow::renderTexture - Set texture alpha mod error: %s\n", SDL_GetError()); fflush(stderr);
    } 

    if(!SDL_RenderTextureRotated(renderer, texture, source, &dst, -angle, NULL, SDL_FLIP_NONE))
    {
    	fprintf(stderr, "aWindow::renderTexture - Render texture rotated error: %s\n", SDL_GetError()); fflush(stderr);
    }

}

void aWindow::updateSize(int nWidth, int nHeight)
{
	SDL_DestroyTexture(screenTexture);
	screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, nWidth*resolutionMultiplier, nHeight*resolutionMultiplier);
	if(screenTexture == NULL)
	{
		fprintf(stderr, "aWindow::updateSize - Create texture error: %s\n", SDL_GetError()); fflush(stderr);
	}
}