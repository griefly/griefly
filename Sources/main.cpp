#pragma comment (lib, "Libs/SDLmain.lib")
#pragma comment (lib, "Libs/SDL.lib")
#pragma comment (lib, "Libs/SDL_image.lib")
#pragma comment (lib, "Libs/SDL_net.lib")
#pragma comment (lib, "Libs/SDL_ttf.lib")
#pragma comment (lib, "Libs/SDL_gfx.lib")
#pragma comment (lib, "Libs/PocoFoundation.lib")
#pragma comment (lib, "Libs/PocoNet.lib")

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_net.h>
#include <SDL_thread.h>
#include "MapClass.h"
#include <iostream>

int main(int argc, char *argv[])
{   
    Mode mode = CLIENT;
    if(argc >= 2 && std::string(argv[1]) == "-server")
    {
        SYSTEM_STREAM << "Set SERVER mode\n";
        mode = SERVER;
    }
    else
        SYSTEM_STREAM << "Set CLIENT mode\n";
    std::string adrs = "127.0.0.1";

    if (argc >= 3 && std::string(argv[2]) == "-nodraw")
        NODRAW = true;

    if(mode == CLIENT)
    {
        SYSTEM_STREAM << "Input ip\n";
        std::cin >> adrs;
    }
    Manager man(mode, adrs);
    SYSTEM_STREAM << "Begin init world\n";
    man.initWorld();
    SYSTEM_STREAM << "End init world\n";
    man.process();
    return 0; 
}
