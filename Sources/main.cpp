#pragma comment (lib, "Libs/SDLmain.lib")
#pragma comment (lib, "Libs/SDL.lib")
#pragma comment (lib, "Libs/SDL_image.lib")
#pragma comment (lib, "Libs/SDL_ttf.lib")
#pragma comment (lib, "Libs/SDL_net.lib")

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_thread.h>

#include <iostream>

#include "MapClass.h"
#include "Debug.h"

int main(int argc, char *argv[])
{   
    std::string adrs = "127.0.0.1";

    if (argc >= 2 && std::string(argv[1]) == "-nodraw")
        NODRAW = true;

    if (argc >= 3)
        Debug::SetUniqueName(std::string(argv[2]));

    // SYSTEM_STREAM << "Input ip\n";
    // std::cin >> adrs;

    Manager man(adrs);
    SYSTEM_STREAM << "Begin init world\n";
    man.initWorld();
    SYSTEM_STREAM << "End init world\n";
    man.process();
    return 0; 
}
