#include "calm_win.h"

#include "core/window.h"
#include "core/device.h"

// TODO: Significant error handling

int main()
{
    calm::Window w(500, 500, "calm engine");
    calm::Device device(w.get_hwnd());

    SDL_Event event;
    while (!w.should_close())
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                w.close();

            device.present();
        }
    }

    return 0;
}
