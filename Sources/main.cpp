#pragma comment (lib, "Libs/SDLmain.lib")
#pragma comment (lib, "Libs/SDL.lib")
#pragma comment (lib, "Libs/SDL_image.lib")
#pragma comment (lib, "Libs/SDL_ttf.lib")
#pragma comment (lib, "Libs/SDL_net.lib")

#ifndef _DEBUG
#pragma comment (lib, "Libs/sfml-main.lib")
#pragma comment (lib, "Libs/sfml-audio.lib")
#endif

#ifdef _DEBUG
#pragma comment (lib, "Libs/sfml-main-d.lib")
#pragma comment (lib, "Libs/sfml-audio-d.lib")
#endif

#pragma comment (lib, "Libs/zdll.lib")
#pragma comment (lib, "Libs/zlibwapi.lib")

#include <iostream>

#include "MapClass.h"
#include "Debug.h"

int main(int argc, char *argv[])
{   
    std::string adrs = "127.0.0.1";

    bool is_auto_p = false;
    if (argc >= 2 && std::string(argv[1]) == "-nodraw")
        NODRAW = true; 
    else if (argc >= 2 && std::string(argv[1]) == "-auto")
        is_auto_p = true;
    else if (argc >= 2)
        adrs = std::string(argv[1]);

    if (argc >= 3)
        Debug::SetUniqueName(std::string(argv[2]));

    Manager man(adrs);
    SYSTEM_STREAM << "Begin init world\n";
    man.initWorld();
    if (is_auto_p)
        man.ToogleAutoplay();
    SYSTEM_STREAM << "End init world\n";
    man.process();
    return 0; 
}
