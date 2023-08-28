#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "math.hpp"
#include "aWindow.hpp"
#include "aCamera.hpp"
#include "utility.hpp"
#include "simulation.hpp"

int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	aWindow mainWindow("particles", 1920/2, 1080/2);
	aCamera mainCamera(1920/2, 1080/2, &mainWindow);
	simulationContainer container(&mainWindow);

	grid mainGrid(vector2f(0, 0), 101, 101, 5, 5);

	//container.addParticle(vector2f(-20, -20), 1);

	//container.getParticle(0) -> velocity = {0, 0};


	SDL_Event event;

	const float timeStep = 0.01f;
	float accumulator = 0.0f;
	float currentTime = SDL_GetTicks() * 0.001f;

	bool running = true;

	while (running)
	{
		int startTicks = SDL_GetTicks();
		float newTime = SDL_GetTicks() * 0.001f;
		float frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;
		while (accumulator > + timeStep)
		{
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT)
					{		
						int x, y;
						SDL_GetMouseState(&x, &y);	
						container.placeParticle(mainCamera.screenToWorld(&mainWindow, vector2f(x, y)), 1, 0);
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						int x, y;
						SDL_GetMouseState(&x, &y);	
						container.placeParticle(mainCamera.screenToWorld(&mainWindow, vector2f(x, y)), 1, 1);
					}
					break;
				case SDL_KEYDOWN:
						switch (event.key.keysym.sym)
						{
							case SDLK_F11:
								mainWindow.switchFullscreen();
								break;
							case SDLK_SPACE:
								container.switchRunning();
								break;
						}
				case SDL_WINDOWEVENT:
					 if(event.window.event == SDL_WINDOWEVENT_RESIZED)
					 {
					 	int w, h;
					 	mainWindow.getSize(&w, &h);
					 	mainWindow.updateSize(w, h);
					 	mainCamera.setSize(w, h);
					 }

				}
			}
			accumulator = timeStep;
		}

		const Uint8 *state = SDL_GetKeyboardState(NULL);

			
			if(state[SDL_SCANCODE_A])
			    mainCamera.moveCamera(vector2f(0.5, 0));
			if(state[SDL_SCANCODE_D])
			    mainCamera.moveCamera(vector2f(-0.5, 0));
			if(state[SDL_SCANCODE_W])
			    mainCamera.moveCamera(vector2f(0, 0.5));
			if(state[SDL_SCANCODE_S])
			    mainCamera.moveCamera(vector2f(0, -0.5));
			if(state[SDL_SCANCODE_E])
			    mainCamera.zoomCamera(0.1);
			if(state[SDL_SCANCODE_Q])
			    mainCamera.zoomCamera(-0.1);
			if(state[SDL_SCANCODE_F])
			{
				int x, y;
				SDL_GetMouseState(&x, &y);	
				container.addParticle(mainCamera.screenToWorld(&mainWindow, vector2f(x, y)), 1);
			}
		mainWindow.clear();

		mainCamera.updateZoom();
		mainCamera.updatePosition();

		mainGrid.render(&mainCamera);

		container.update();
		

		container.render(&mainCamera);

		mainWindow.display();

		int frameTicks = SDL_GetTicks() - startTicks;
		if (frameTicks < 1000 / (mainWindow.getRefreshRate()))
			SDL_Delay(1000 / (mainWindow.getRefreshRate()) - frameTicks);
	}

	TTF_Quit();
	mainWindow.cleanUp();
	SDL_Quit();
	return 0;
}
