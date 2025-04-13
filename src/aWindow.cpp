#include <SDL3/SDL.h>

#include "aWindow.hpp"

aWindow::aWindow(const char* title, int width, int height)
	: window(NULL), renderer(NULL)
{
	resolutionMultiplier = 2;

    background = {30, 30, 30, 255};
    fullscreen = false;

	windowWidth = width;
	windowHeight = height;
	window = SDL_CreateWindow(title, width, height, 0);
	if (window == NULL)
	{
        log::error("aWindow::aWindow - Create window error: {}", SDL_GetError());
    }

	if(!SDL_SetWindowResizable(window, true))
	{
		log::error("aWindow::aWindow - Set window resizable error: {}", SDL_GetError());
	}

	renderer = SDL_CreateRenderer(window, NULL);
	if (renderer == NULL)
	{
        log::error("aWindow::aWindow - Create renderer error: {}", SDL_GetError());
    }

	screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)(width*resolutionMultiplier), (int)(height*resolutionMultiplier));
	if (screenTexture == NULL)
	{
        log::error("aWindow::aWindow - Create texture error: {}", SDL_GetError());
    }

	if(!SDL_SetRenderTarget(renderer, screenTexture))
    {
        log::error("aWindow::aWindow - Set render target error: {}", SDL_GetError());
    }

	if(!SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a))
    {
        log::error("aWindow::aWindow - Set render draw color error: {}", SDL_GetError());
    }

	if(!SDL_RenderClear(renderer))
    {
        log::error("aWindow::aWindow - Render clear error: {}", SDL_GetError());
    }

	if(!SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND))
    {
        log::error("aWindow::aWindow - Set render draw blend mode error: {}", SDL_GetError());
    }

	textEngine = TTF_CreateSurfaceTextEngine();
    if (textEngine == NULL)
    {
        log::error("aWindow::aWindow - Create surface text engine error: {}", SDL_GetError());
    }

    for (int size = 8; size <= 128; size += 2)
    {
        TTF_Font* tempFont = TTF_OpenFont("res/font.ttf", float(size));
        if (tempFont == NULL) {
            log::error("aWindow::aWindow - Open font error: {}", SDL_GetError());
            continue;
        } 
        else 
        {
            fonts[size] = tempFont;
        }
    }

    log::info("aWindow::aWindow - Using renderer: {}", SDL_GetRendererName(renderer));

    log::info("aWindow::aWindow - Constructor finished successfully.");
}

SDL_Texture* aWindow::loadTexture(const char* filePath)
{
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, filePath);
	if (texture == NULL)
	{
		log::error("aWindow::loadTexture - Load texture error: {}", SDL_GetError());
	}
	return texture;
}

int aWindow::getRefreshRate()
{
    int displayIndex = SDL_GetDisplayForWindow(window);
    if(displayIndex == 0)
    {
    	log::error("aWindow::getRefreshRate - Get display for window error: {}", SDL_GetError());
        return 0;
    }

	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(displayIndex);
	if(mode == NULL)
    {
    	log::error("aWindow::getRefreshRate - Get current display mode error: {}", SDL_GetError());
        return 0;
    }
	return int(mode -> refresh_rate);
}

double aWindow::getResolutionMultiplier()
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
    	log::error("aWindow::clear - Render clear error: {}", SDL_GetError());
    }
}

void aWindow::display()
{
    if (!SDL_SetRenderTarget(renderer, NULL))
    {
        log::error("aWindow::display - Set render target error: {}", SDL_GetError());
    }

    int pixelW = 0, pixelH = 0;

    if (!SDL_GetWindowSizeInPixels(window, &pixelW, &pixelH))
    {
        log::error("aWindow::display - Get window size in pixels error: {}", SDL_GetError());
    }

    SDL_Rect viewport = {0, 0, pixelW, pixelH};

    if (!SDL_SetRenderViewport(renderer, &viewport))
    {
        log::error("aWindow::display - Set render viewport error: {}", SDL_GetError());
    }

    if (!SDL_RenderTexture(renderer, screenTexture, NULL, NULL))
    {
        log::error("aWindow::display - Render texture error: {}", SDL_GetError());
    }

    if (!SDL_RenderPresent(renderer))
    {
        log::error("aWindow::display - Render present error: {}", SDL_GetError());
    }

    if (!SDL_SetRenderTarget(renderer, screenTexture))
    {
        log::error("aWindow::display - Set render target error: {}", SDL_GetError());
    }
}

void aWindow::renderRect(SDL_FRect rect, SDL_Color color)
{
    rect = {rect.x * float(resolutionMultiplier),
            rect.y * float(resolutionMultiplier),
            rect.w * float(resolutionMultiplier),
            rect.h * float(resolutionMultiplier)};

    int ww, wh;
    getSize(&ww, &wh);
    ww = int(ww * resolutionMultiplier);
    wh = int(wh * resolutionMultiplier);
    
    if(rect.x > ww || rect.y > wh || rect.x+rect.w < 0 || rect.y+rect.h < 0)
    {
        return;
    }

    if(!SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a))
    {
    	log::error("aWindow::renderRect - Set render draw color error: {}", SDL_GetError());
    }
    if(!SDL_RenderFillRect(renderer, &rect))
    {
    	log::error("aWindow::renderRect - Render fill rect error: {}", SDL_GetError());
    }
    if(!SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a))
    {
    	log::error("aWindow::renderRect - Set render draw color error: {}", SDL_GetError());
    }
}


void aWindow::renderDisc(vector2d position, int radius, SDL_Color color)
{
    position *= float(resolutionMultiplier);
    radius *= int(resolutionMultiplier);

    int offsetx, offsety, d;

    offsetx = 0;
    offsety = radius;
    d = radius -1;

    if(!SDL_SetRenderDrawColor(renderer, color.r,  color.g,  color.b,  color.a))
    {
    	log::error("aWindow::renderDisc - Set render draw color error: {}", SDL_GetError());
    }

    while (offsety >= offsetx)
    {
        if(!SDL_RenderLine(renderer, float(position.x - offsety), float(position.y + offsetx), float(position.x + offsety), float(position.y + offsetx)))
        {
        	log::error("aWindow::renderDisc - Render line error: {}", SDL_GetError());
        }
        if(!SDL_RenderLine(renderer, float(position.x - offsetx), float(position.y + offsety), float(position.x + offsetx), float(position.y + offsety)))
        {
        	log::error("aWindow::renderDisc - Render line error: {}", SDL_GetError());
        }
        if(!SDL_RenderLine(renderer, float(position.x - offsetx), float(position.y - offsety), float(position.x + offsetx), float(position.y - offsety)))
        {
        	log::error("aWindow::renderDisc - Render line error: {}", SDL_GetError());
        }
        if(!SDL_RenderLine(renderer, float(position.x - offsety), float(position.y - offsetx), float(position.x + offsety), float(position.y - offsetx)))
        {
        	log::error("aWindow::renderDisc - Render line error: {}", SDL_GetError());
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
    	log::error("aWindow::renderDisc - Set render draw color error: {}", SDL_GetError());
    }
}

void aWindow::getSize(int *nw, int *nh)
{
    *nw = windowWidth;
    *nh = windowHeight;
}

SDL_Texture* aWindow::getScreenTexture()
{
	return screenTexture;
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
			log::error("aWindow::switchFullscreen - Set window fullscreen error: {}", SDL_GetError());
		}
    }
    else
    {
        if(SDL_SetWindowFullscreen(window, true))
    	{
    		fullscreen = true;
		}else
		{
			log::error("aWindow::switchFullscreen - Set window fullscreen error: {}", SDL_GetError());
		}
    }
}

void aWindow::renderText(vector2d position, double height, std::string alignment, std::string text, SDL_Color color)
{
	position *= float(resolutionMultiplier);
	height *= float(resolutionMultiplier);

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
    		log::error("aWindow::renderText - Get string size error: {}", SDL_GetError());
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
		ww = int(ww * resolutionMultiplier);
		wh = int(wh * resolutionMultiplier);
		
		if(rect.x > ww || rect.y > wh || rect.x+rect.w < 0 || rect.y+rect.h < 0)
		{
			continue;
		}

	    TTF_Text* text = TTF_CreateText(textEngine, bestFont, line.c_str(), line.size());
	    if(text==NULL)
	    {
	    	log::error("aWindow::renderText - Create text error: {}", SDL_GetError());
	    }

		SDL_Surface *surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
		if(surface==NULL)
		{
	    	log::error("aWindow::renderText - Create surface error: {}", SDL_GetError());
		}

		if(!TTF_DrawSurfaceText(text, 0, 0, surface))
		{
			log::error("aWindow::renderText - Draw surface text error: {}", SDL_GetError());
		}
		
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
		if(texture==NULL)
		{
	    	log::error("aWindow::renderText - Create texture from surface error: {}", SDL_GetError());
		}

		if(!SDL_RenderTexture(renderer, texture, NULL, &rect))
		{
			log::error("aWindow::renderText - Render texture error: {}", SDL_GetError());
		}

		TTF_DestroyText(text);
		SDL_DestroySurface(surface);
		SDL_DestroyTexture(texture);
	}
}

void aWindow::renderTexture(SDL_Texture* texture, SDL_FRect* source, SDL_FRect* destination, double angle, SDL_Color color)
{
	SDL_FRect dst = *destination;

	dst.x *= float(resolutionMultiplier);
	dst.y *= float(resolutionMultiplier);
	dst.w *= float(resolutionMultiplier);
	dst.h *= float(resolutionMultiplier);

	if(dst.w < 1)
		dst.w = 1;
	if(dst.h < 1)
		dst.h = 1;

    int ww, wh;
    getSize(&ww, &wh);
    ww = int(ww * resolutionMultiplier);
    wh = int(wh * resolutionMultiplier);
    
    if(dst.x > ww || dst.y > wh || dst.x+dst.w < 0 || dst.y+dst.h < 0)
    {
        return;
    }

    if(!SDL_SetTextureColorMod(texture, color.r, color.g, color.b))
    {
    	log::error("aWindow::renderTexture - Set texture color mod error: {}", SDL_GetError());
    }

    if(!SDL_SetTextureAlphaMod(texture, color.a))
    {
    	log::error("aWindow::renderTexture - Set texture alpha mod error: {}", SDL_GetError());
    } 

    if(!SDL_RenderTextureRotated(renderer, texture, source, &dst, -angle, NULL, SDL_FLIP_NONE))
    {
    	log::error("aWindow::renderTexture - Render texture rotated error: {}", SDL_GetError());
    }
}

void aWindow::updateSize(int nWidth, int nHeight)
{
    windowWidth = nWidth;
    windowHeight = nHeight;

    int textureW = static_cast<int>(nWidth * resolutionMultiplier);
    int textureH = static_cast<int>(nHeight * resolutionMultiplier);

    if (screenTexture != nullptr)
    {
        SDL_DestroyTexture(screenTexture);
        screenTexture = nullptr;
    }

    screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, textureW, textureH);
    if (screenTexture == nullptr)
    {
        log::error("aWindow::updateSize - Create texture error: {}", SDL_GetError());
        screenTexture = nullptr;
    } else
    {
        if (!SDL_SetRenderTarget(renderer, screenTexture))
        {
            log::error("aWindow::updateSize - Set render target error: {}", SDL_GetError());
        }
        
        if (!SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a))
        {
            log::error("aWindow::updateSize - Set render draw color error: {}", SDL_GetError());
        }

        if (!SDL_RenderClear(renderer))
        {
            log::error("aWindow::updateSize - Render clear error: {}", SDL_GetError());
        }
    }
}