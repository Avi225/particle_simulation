#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_main.h>
#include <cstdlib>

#include "manager.hpp"

int main(int argc, char* args[])
{
	manager* m = new manager();
	m -> loop();
	delete m;
	exit(0);
	return 0;
}