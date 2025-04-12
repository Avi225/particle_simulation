#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_main.h>
#include <cstdlib>

#include "manager.hpp"

int main(int argc, char* args[])
{
    fmt::print("\033[?1049h");  // Enter alternate screen buffer
    fmt::print("\033[?25l");     // Hide cursor
    fflush(stdout);

    manager* m = new manager();
    m->loop();

    fmt::print("\033[?1049l");  // Exit alternate screen buffer
    fmt::print("\033[?25h");     // Show cursor
    fflush(stdout);
    
    delete m;
    exit(0);
    return 0;
}