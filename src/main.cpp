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
#include "interface.hpp"

float abc = 15;

menu* generateMenu();
simulationContainer* generateSimulationContainer();

int main(int argc, char* args[])
{
	// Initialize SDL, SDL_image, and SDL_ttf libraries
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	// Create a main window and camera
	aWindow mainWindow("particles", 1920/2, 1080/2);
	aCamera mainCamera(1920/2, 1080/2, &mainWindow);

	// Create simulation container and grid
	
	simulationContainer* container = generateSimulationContainer();
	menu* mainMenu = generateMenu();
	grid mainGrid(vector2f(0, 0), 101, 101, 5, 5);

	// Initialize SDL_Event for handling events
	SDL_Event event;

	const float timeStep = 0.01f;
	float accumulator = 0.0f;
	float currentTime = SDL_GetTicks() * 0.001f;

	bool running = true;
	bool ticker = false;
	while (running)
	{
		int startTicks = SDL_GetTicks();
		float newTime = SDL_GetTicks() * 0.001f;
		float frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;
		while (accumulator > + timeStep)
		{
			// Event handling loop
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					// Start placing a particle, this will be it's position
					if (event.button.button == SDL_BUTTON_LEFT)
					{		
						int x, y;
						SDL_GetMouseState(&x, &y);	
						container -> placeParticle(mainCamera.screenToWorld(vector2f(x, y)), 4, 0);
					}
					break;
				case SDL_MOUSEBUTTONUP:
					// Finish placing a particle, it's velocity will be pointing towards this point
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						int x, y;
						SDL_GetMouseState(&x, &y);	
						container -> placeParticle(mainCamera.screenToWorld(vector2f(x, y)), 4, 1);
					}
					break;
				case SDL_KEYDOWN:
						// Handle key presses
						switch (event.key.keysym.sym)
						{
							case SDLK_F11:
								// Toggle fullscreen mode
								mainWindow.switchFullscreen();
								int w, h;
								mainWindow.getSize(&w, &h);
								mainWindow.updateSize(w, h);
								mainCamera.setSize(w, h);
								break;
							case SDLK_SPACE:
								// Toggle simulation running
								container -> switchRunning();
								break;
							case SDLK_r:
								// Advance the simulation by 1 tick
								ticker = true;
								break;
						}
				case SDL_WINDOWEVENT:
					// Handle window events, like resizing
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

		// Check keyboard state and perform camera movement/zoom
		const Uint8 *state = SDL_GetKeyboardState(NULL);
			
		if(state[SDL_SCANCODE_LSHIFT])
		{
			if(state[SDL_SCANCODE_A])
		   		mainCamera.moveCamera(vector2f(1.5, 0));
			if(state[SDL_SCANCODE_D])
			    mainCamera.moveCamera(vector2f(-1.5, 0));
			if(state[SDL_SCANCODE_W])
			    mainCamera.moveCamera(vector2f(0, 1.5));
			if(state[SDL_SCANCODE_S])
			    mainCamera.moveCamera(vector2f(0, -1.5));
		}
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
			container -> addParticle(mainCamera.screenToWorld(vector2f(x, y)), .8);
		}

		 // Advancing by 1 tick logic
		if(ticker && container -> getRunning() == false)
			container -> switchRunning();

		mainWindow.clear();

		// Update camera parameters
		mainCamera.updateZoom();
		mainCamera.updatePosition();

		// Render
		mainGrid.render(&mainCamera);

		container -> update();
		container -> render(&mainCamera);

		mainMenu -> render(&mainCamera);

		mainWindow.display();

		if(ticker)
			container -> switchRunning();

		ticker = false;

		// Delay to maintain frame rate at monitor refresh rate
		int frameTicks = SDL_GetTicks() - startTicks;
		if (frameTicks < 1000 / (mainWindow.getRefreshRate()))
			SDL_Delay(1000 / (mainWindow.getRefreshRate()) - frameTicks);
	}

	// Cleanup and quit SDL, SDL_image, and SDL_ttf libraries
	mainWindow.cleanUp();
	TTF_Quit();
	SDL_Quit();
	return 0;
}

menu* generateMenu()
{
	menu* mainMenu = new menu(vector2f(0, 0));
	mainMenu -> insertElement(new menuText("test", 1));
	mainMenu -> insertElement(new menuText("test", 1));
	mainMenu -> insertElement(new menuText("test3", 1));
	mainMenu -> insertElement(new menuText("test4", 1));
	mainMenu -> insertElement(new menuSliderF(&abc, 0, 20));
	return(mainMenu);
}

simulationContainer* generateSimulationContainer()
{
	simulationContainer* container = new simulationContainer;
	// for(int i = 0; i < 10; ++i)
	//  	for (int ii = 0; ii < 40; ++ii)
	//  		container -> addParticle(vector2f(i*1.6-60, ii*1.6), 0.8);

	container -> addStaticPoint(vector2f(-20, 0));
	container -> addStaticPoint(vector2f(-20, 20));
	container -> addStaticPoint(vector2f(20, 20));
	container -> addStaticPoint(vector2f(20, 0));
	container -> addStaticPoint(vector2f(100, 0));

	container -> addStaticLine(0, 1);
	container -> addStaticLine(1, 2);
	container -> addStaticLine(2, 3);
	container -> addStaticLine(3, 4);

	return(container);
}
