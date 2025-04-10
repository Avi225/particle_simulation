#include "manager.hpp"

manager::manager()
{
	newTime = 0;
	frameTime = 0;
	currentTime = 0;
	accumulator = 0;

	fpsCap = 0;
	frameCount = 0;
	fpsTimer = 0;
	fps = 0;
	displayFps = "";

	timeStep = 0;
	tickCount = 0;
	tpsTimer = 0;
	tps = 0;
	displayTps = "";

	state = 1;

	keyState = nullptr;

	event = {};

	running = false;

	mainGrid = nullptr;

	p = nullptr;
	container = nullptr;
	particleSpawnCount = 1;
	displayParticleCount = "";
	displaySimulationState = "";

	camera = nullptr;
	window = nullptr;

	debugMenu = nullptr;
	controlsMenu = nullptr;
}

void manager::update()
{
	switch(state)
	{
		case 0:
		{
			running = false;
			break;
		}
		case 1:
		{
			float mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);
			int w, h;
			camera -> getSize(&w, &h);

			// Check keyboard state and perform camera movement/zoom
			vector2d cameraMove = {0, 0};
			double cameraZoom = 0.0;

			if(keyState[SDL_SCANCODE_A])
			    cameraMove.x += 0.5;
			if(keyState[SDL_SCANCODE_D])
			    cameraMove.x += -0.5;
			if(keyState[SDL_SCANCODE_W])
			    cameraMove.y += 0.5;
			if(keyState[SDL_SCANCODE_S])
			    cameraMove.y += -0.5;
			if(keyState[SDL_SCANCODE_E])
			    cameraZoom += 0.05;
			if(keyState[SDL_SCANCODE_Q])
			    cameraZoom += -0.05;
			if(keyState[SDL_SCANCODE_LSHIFT])
			{
				cameraMove *= 2;
				cameraZoom *= 2;
			}
			camera -> moveCamera(cameraMove);
			camera -> zoomCamera(cameraZoom);

			// Spawn particles when F is held
			if(keyState[SDL_SCANCODE_F])
			{
				for (int i = 0; i < particleSpawnCount; ++i)
					container -> addParticle(camera -> screenToWorld(vector2d(mouseX+double(i)/1000, mouseY+double(i)/1000)), 5);	
			}		

			camera -> updateZoom();
			camera -> updatePosition();
			if(container -> getRunning())
				container -> update();
			if(debugMenu != nullptr)
				debugMenu -> update(camera, {0, 0, float(w), float(h)}, debugMenu);
			if(controlsMenu != nullptr)
				controlsMenu -> update(camera, {0, 0, float(w), float(h)}, controlsMenu);
			break;
		}
		default:
		{
			log::error("Error: manager state not found\n");
			break;
		}
	}
}

void manager::render()
{
	window -> clear();	

	switch(state)
	{
	case 0:
		{
			break;
		}
	case 1:
		{
			int w, h;
			camera -> getSize(&w, &h);
			float mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);
			
			mainGrid -> render(camera);

			container -> render(camera);
			container -> renderQuadTree(camera, {mouseX, mouseY});

			if(debugMenu != nullptr)
			 	debugMenu -> render(camera, {0, 0, float(w), float(h)}, debugMenu);
			if(controlsMenu != nullptr)
			 	controlsMenu -> render(camera, {0, 0, float(w), float(h)}, controlsMenu);
			break;
		}
	default:
		{
			log::error("Error: manager state not found\n");
			break;
		}
	}

	window -> display();
}

void manager::init()
{
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	newTime = 0.0;
	frameTime = 0.0;
	currentTime = SDL_GetTicks() * 0.001;
	accumulator = 0.0;


	frameCount = 0;
	fpsTimer = 0.0;
	fps = 0;
	displayFps = "fps: 0";

	timeStep = 0.01;
	tickCount = 0;
	tpsTimer = 0.0;
	tps = 0;
	displayTps = "tps: 0";

	running = true;

	keyState = SDL_GetKeyboardState(NULL);

	window = new aWindow("particles", 1280, 720);
	camera = new aCamera(1280, 720, window);

	fpsCap = window -> getRefreshRate(); 

	int w, h;
	window -> getSize(&w, &h);
	window -> updateSize(w, h);
	camera -> setSize(w, h);
	camera -> loadTextures();

	container = new simulationContainer();

	mainGrid = new grid(vector2d(0, 0), 1024, 1024, 16, 16);

	p = new parser();

	loadMenus();
}

void manager::loadMenus()
{
	debugMenu = p -> parse("res/debugMenu.ui");
	controlsMenu = p -> parse("res/controlsMenu.ui");
	menuText* menu = nullptr;

	menu = dynamic_cast<menuText*>(debugMenu->getById("fps"));
	if (menu)
	{
		delete menu->text;
		menu->textOwned = false;
	    menu->text = &displayFps;
	}
	menu = nullptr;
	menu = dynamic_cast<menuText*>(debugMenu->getById("tps"));
	if (menu)
	{
		delete menu->text;
		menu->textOwned = false;
	    menu->text = &displayTps;
	}
	menu = nullptr;
	menu = dynamic_cast<menuText*>(debugMenu->getById("count"));
	if (menu)
	{
		delete menu->text;
		menu->textOwned = false;
	    menu->text = &displayParticleCount;
	}
	menu = nullptr;
	menu = dynamic_cast<menuText*>(debugMenu->getById("state"));
	if (menu)
	{
		delete menu->text;
		menu->textOwned = false;
	    menu->text = &displaySimulationState;
	}
	menu = nullptr;
	menu = dynamic_cast<menuText*>(debugMenu->getById("log"));
	if (menu)
	{
		delete menu->text;
		menu->textOwned = false;
	    menu->text = log::getString();
	}
}

// Color division is sick, you know what unites us?
// Color of the flowers, when we hit the graveyard.

void manager::loop()
{
	init();
	while (running)
	{
		newTime = SDL_GetTicks() * 0.001;
	    frameTime = newTime - currentTime;
	    currentTime = newTime;
	    accumulator += frameTime;

		while (accumulator >= timeStep)
		{
			log::clear();

			float mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);

			// Event handling loop
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
					case SDL_EVENT_QUIT:
						running = false;
						break;

					case SDL_EVENT_MOUSE_BUTTON_DOWN:
						// Start placing a particle, this will be it's position
						if (event.button.button == SDL_BUTTON_LEFT)
						{		
							container -> placeParticle(camera -> screenToWorld(vector2d(mouseX, mouseY)), 50, 0);
						}
						break;

					case SDL_EVENT_MOUSE_BUTTON_UP:
						// Finish placing a particle, it's velocity will be pointing towards this point
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							container -> placeParticle(camera -> screenToWorld(vector2d(mouseX, mouseY)), 50, 1);
						}
						break;
					
					case SDL_EVENT_KEY_DOWN:
						switch (event.key.key)
						{
							case SDLK_F11:
								// Toggle fullscreen mode
								window -> switchFullscreen();
								int w, h;
								window -> getSize(&w, &h);
								window -> updateSize(w, h);
								camera -> setSize(w, h);
								break;
							case SDLK_SPACE:
								// Toggle simulation running
								container -> switchRunning();
								break;
							case SDLK_R:
								// Advance the simulation by 1 tick
								container -> update();
								break;
							case SDLK_Z:
								// Display a debug overlay of a hovered quadrant
								container -> select(camera);
								break;
							}
						break;
					case SDL_EVENT_WINDOW_RESIZED:  
						int w, h;
						window -> getSize(&w, &h);
						window -> updateSize(w, h);
						camera -> setSize(w, h);
						break;
					case SDL_EVENT_MOUSE_WHEEL:
						if(event.wheel.y > 0)
							particleSpawnCount += 1;
						if(event.wheel.y < 0)
							particleSpawnCount += (particleSpawnCount > 1) ? -1 : 0;
						break;
				}	
			}
			update();

			tickCount++;
		    
			accumulator -= timeStep;
		}

		render();

        frameCount++;
	    fpsTimer += frameTime;
	    if (fpsTimer >= 1.0)
	    {
	        fps = frameCount;
	        frameCount = 0;
	        fpsTimer -= 1.0;
	        displayFps = "fps: " + std::to_string(fps);
	    }

	    tpsTimer += frameTime;
	    if (tpsTimer >= 1.0)
	    {
	        tps = tickCount;
	        tickCount = 0;
	        tpsTimer -= 1.0;
	        displayTps = "tps: " + std::to_string(tps);
	    }

	    displayParticleCount = "particles: " + std::to_string(container -> getParticleCount());
		displaySimulationState = "running: " + std::string((container -> getRunning()) ? "true" : "false");

        if(fpsCap > 0)
		{
			int frameDelay = int((1000 / fpsCap) - (SDL_GetTicks() - (newTime * 1000)));
		    if (frameDelay > 0)
		        SDL_Delay(frameDelay); 
		}
	}
	cleanUp();
}

void manager::cleanUp()
{
	delete mainGrid;
	delete p;

	debugMenu -> cleanUp();
	controlsMenu -> cleanUp();
	delete debugMenu;
	delete controlsMenu;
	
	container -> cleanUp();
	delete container;

	camera -> cleanUp();
	window -> cleanUp();
	delete camera;
	delete window;

	TTF_Quit();
	SDL_Quit();
}
