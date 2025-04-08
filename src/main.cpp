#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
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
