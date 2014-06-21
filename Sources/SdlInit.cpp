#include "SdlInit.h"

#include <SDL.h>
#include <SDL_net.h>
#include <SDL_ttf.h>

#include "constheader.h"
#include "sprite.h"

bool InitSDL()
{
    Uint32 rmask, gmask, bmask, amask;
    SetMasks(&rmask, &gmask, &bmask, &amask);

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    { 
        SYSTEM_STREAM << "Unable to init SDL: " << SDL_GetError() << std::endl; 
        SDL_Delay(10000);
        return false;
    }
    atexit(SDL_Quit);

    int result = TTF_Init();
    if (result < 0)
    {
        SYSTEM_STREAM << "Unable to init TTF: " << result << std::endl;
        SDL_Delay(10000);
        return false;
    }
    atexit(TTF_Quit);

    result = SDLNet_Init();
    if (result < 0)
    {
        SYSTEM_STREAM << "Unable to init SDLNet: " << result << std::endl;
        SDL_Delay(10000);
        return false;
    }
    atexit(SDLNet_Quit);
    return true;
}