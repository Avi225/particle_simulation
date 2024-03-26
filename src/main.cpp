#include <iostream>
#include <vector>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_thread.h>

#include "math.hpp"
#include "aWindow.hpp"
#include "aCamera.hpp"
#include "utility.hpp"
#include "simulation.hpp"
#include "interface.hpp"

menu* generateMenu(simulationContainer* container);
simulationContainer* generateSimulationContainer();
void renderScene(aCamera* camera, aWindow* window, grid* mainGrid, simulationContainer* container, menu* mainMenu);

double fps = 0;
double avgFps = 0;
double totalFpsAvg = 0;
double totalFrames = 0;
int secFps = 0;
int particleSpawnCount = 1;

int main(int argc, char* args[])
{
	// Initialize SDL, SDL_image, and SDL_ttf libraries
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	int s = 200;

	// Create a main window and camera
	aWindow mainWindow("particles", 1920/2, 1080/2);
	aCamera mainCamera(1920/2, 1080/2, &mainWindow);

	// Create simulation container and grid
	simulationContainer* container = generateSimulationContainer();
	menu* mainMenu = generateMenu(container);
	grid mainGrid(vector2d(0, 0), 501, 501, 20, 20);

	// for (int x = -100; x < 100; ++x)
	// {
	// 	for (int y = -200; y < -190; ++y)
	// 	{
	// 		container -> addParticle(vector2d(x, y), 0.5);
	// 	}
	// }
	
	// Initialize SDL_Event for handling events
	SDL_Event event;

	const double timeStep = 0.01f;
	double accumulator = 0.0f;
	double currentTime = SDL_GetTicks() * 0.001f;
	Uint32 fpsFrame;
	int frames = 0;

	bool running = true;
	while (running)
	{
		int startTicks = SDL_GetTicks();
		double newTime = SDL_GetTicks() * 0.001f;
		double frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;

		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

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
						container -> placeParticle(mainCamera.screenToWorld(vector2d(mouseX, mouseY)), 30, 0);
					}
					break;

				case SDL_MOUSEBUTTONUP:
					// Finish placing a particle, it's velocity will be pointing towards this point
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						container -> placeParticle(mainCamera.screenToWorld(vector2d(mouseX, mouseY)), 30, 1);
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
							container -> update();
							break;
						case SDLK_z:
							container -> select(&mainCamera);
							break;
					}
					break;
				case SDL_WINDOWEVENT:
					// Handle window events, like resizing
					if(event.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						int w, h;
						mainWindow.getSize(&w, &h);
						mainWindow.updateSize(w, h);
						mainCamera.setSize(w, h);
					}
					break;
				case SDL_MOUSEWHEEL:
					if(event.wheel.y > 0)
						particleSpawnCount += 1;
					if(event.wheel.y < 0)
						particleSpawnCount += (particleSpawnCount > 0) ? -1 : 0;
					break;
				}
			}
			accumulator = timeStep;
		}

		// Check keyboard state and perform camera movement/zoom
		const Uint8 *state = SDL_GetKeyboardState(NULL);
			
		if(state[SDL_SCANCODE_LSHIFT])
		{
			if(state[SDL_SCANCODE_A])
		   		mainCamera.moveCamera(vector2d(1.5, 0));
			if(state[SDL_SCANCODE_D])
			    mainCamera.moveCamera(vector2d(-1.5, 0));
			if(state[SDL_SCANCODE_W])
			    mainCamera.moveCamera(vector2d(0, 1.5));
			if(state[SDL_SCANCODE_S])
			    mainCamera.moveCamera(vector2d(0, -1.5));
			if(state[SDL_SCANCODE_E])
			    mainCamera.zoomCamera(0.3);
			if(state[SDL_SCANCODE_Q])
			    mainCamera.zoomCamera(-0.3);
		}else 
		{
			if(state[SDL_SCANCODE_A])
			    mainCamera.moveCamera(vector2d(0.5, 0));
			if(state[SDL_SCANCODE_D])
			    mainCamera.moveCamera(vector2d(-0.5, 0));
			if(state[SDL_SCANCODE_W])
			    mainCamera.moveCamera(vector2d(0, 0.5));
			if(state[SDL_SCANCODE_S])
			    mainCamera.moveCamera(vector2d(0, -0.5));
			if(state[SDL_SCANCODE_E])
			    mainCamera.zoomCamera(0.1);
			if(state[SDL_SCANCODE_Q])
			    mainCamera.zoomCamera(-0.1);
		}
		if(state[SDL_SCANCODE_F])
		{
			for (int i = 0; i < particleSpawnCount; ++i)
				container -> addParticle(mainCamera.screenToWorld(vector2d(mouseX+double(i)/1000, mouseY+double(i)/1000)), 5);	
		}

		// Update camera parameters
		mainCamera.updateZoom();
		mainCamera.updatePosition();
    	
    	//Update simulation container
		if(container -> getRunning())
			container -> update();

		//Update main menu
		mainMenu -> updateTabs();

		renderScene(&mainCamera, &mainWindow, &mainGrid, container, mainMenu);

		// Delay to maintain frame rate at monitor refresh rate
		int frameTicks = SDL_GetTicks() - startTicks;
		if (frameTicks < 1000 / mainWindow.getRefreshRate())
			SDL_Delay(1000 / mainWindow.getRefreshRate() - frameTicks);
		fpsFrame = SDL_GetTicks() - startTicks;
		fps = (fpsFrame > 0) ? 1000.0f / fpsFrame : 0.0f;
		avgFps += fps;
		totalFpsAvg += fps;
		totalFrames++;
		frames++;
		if(frames >= fps)
		{
			secFps = int(round(avgFps /= frames));
			avgFps = 0;
			frames = 0;
		}


		s--;
		// if(s <= 0)
	 	// {
	 	// 	printf("%f fps average\n", totalFpsAvg/totalFrames);
	 	// 	break;
	 	// }
	}
	
	//system("pause");
	// Cleanup and quit SDL, SDL_image, and SDL_ttf libraries
	mainWindow.cleanUp();
	TTF_Quit();
	SDL_Quit();
	
	return 0;
}

menu* generateMenu(simulationContainer* container)
{
	menu* mainMenu = new menu(vector2d(16, 16));
	tab* test1 = new tab(vector2d(0, 41), "main");

	test1 -> insertElement(new tabDisplayI(container -> getParticleCount(), 1, "particles: "));
	test1 -> insertElement(new tabDisplayI(&secFps, 1, "fps: "));

	mainMenu -> insertTab(test1);
	//mainMenu -> insertTab(test2);
	return(mainMenu);
}

simulationContainer* generateSimulationContainer()
{
	simulationContainer* container = new simulationContainer;

	// for(int i = 0; i < 10; ++i)
	//  	for (int ii = 0; ii < 40; ++ii)
	//  		container -> addParticle(vector2d(i*1.6-60, ii*1.6), 0.8);

	container -> addStaticPoint(vector2d(-100, 0));
	container -> addStaticPoint(vector2d(100, 0));

	container -> addStaticLine(0, 1);

	return(container);
}

void renderScene(aCamera* camera, aWindow* window, grid* mainGrid, simulationContainer* container, menu* mainMenu)
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	window -> clear();
	
	mainGrid -> render(camera);

	container -> render(camera);
	container -> renderQuadTree(camera, {double(mouseX), double(mouseY)});

	mainMenu -> render(camera);

	window -> display();
}

