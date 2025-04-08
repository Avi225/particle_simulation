#pragma once

#include <thread>
#include <atomic>
#include <chrono>

#include "ThreadPool.h"

#include "math.hpp"
#include "utility.hpp"
#include "parser.hpp"
#include "interface.hpp"
#include "simulation.hpp"

class manager
{
public:
	manager();
	void loadMenus();
	void update();
	void render();
	void init();
	void loop();
	void cleanUp();

private:
	double newTime;
	double frameTime;
	double currentTime;
	double accumulator;

	int fpsCap;
	int frameCount;
	double fpsTimer;
	int fps;
	std::string displayFps;

	double timeStep;
	int tickCount;
	double tpsTimer;
	int tps;
	std::string displayTps;
	
	int state;

	const bool* keyState;

	SDL_Event event;

	bool running;

	grid* mainGrid;

	parser* p;
	simulationContainer* container;
	int particleSpawnCount;
	std::string displayParticleCount;
	std::string displaySimulationState;

	aCamera* camera;
	aWindow* window;

	menu* debugMenu;
	menu* controlsMenu;
};