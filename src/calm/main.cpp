#include <iostream>

#include "calm_win.h"

#include "core/window.h"

int main()
{
    calm::Window w(500, 500, "Hello World");

    SDL_Event event;
    while (!w.should_close())
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                w.close();
        }
    }

    return 0;
}
