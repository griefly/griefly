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
#include "Params.h"
#include "MapEditor.h"
#include "Mob.h"

int main(int argc, char *argv[])
{   
    GetParamsHolder().ParseParams(argc, argv);

    if (!GetParamsHolder().GetParam<bool>("-editor"))
    {
        if (GetParamsHolder().GetParam<bool>("-nodraw"))
            NODRAW = true; 
    
        bool is_auto_p = false;
        if (GetParamsHolder().GetParam<bool>("-auto"))
            is_auto_p = true;

        std::string adrs = "127.0.0.1";
        if (GetParamsHolder().GetParam<bool>("ip"))
            adrs = GetParamsHolder().GetParam<std::string>("ip");

        if (GetParamsHolder().GetParam<bool>("name"))
            Debug::SetUniqueName(GetParamsHolder().GetParam<std::string>("name"));

        Manager man(adrs);
        SYSTEM_STREAM << "Begin init world\n";
        man.initWorld();
        if (is_auto_p)
            man.ToogleAutoplay();
        SYSTEM_STREAM << "End init world\n";
        man.process();
        return 0;
    }
    MapEditor map_editor;
    map_editor.InitWorld();
    map_editor.Run();
    return 0; 
}
