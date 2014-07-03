#pragma comment (lib, "Libs/SDLmain.lib")
#pragma comment (lib, "Libs/SDL.lib")
#pragma comment (lib, "Libs/SDL_image.lib")
#pragma comment (lib, "Libs/SDL_ttf.lib")
#pragma comment (lib, "Libs/SDL_net.lib")
#pragma comment (lib, "Libs/libpng16.lib")

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

#include "Metadata.h"

int main(int argc, char *argv[])
{   

    ImageMetadata im;
    im.Init("icons/human.png");

    GetParamsHolder().ParseParams(argc, argv);

    if (!GetParamsHolder().GetParamBool("-editor"))
    {
        if (GetParamsHolder().GetParamBool("-nodraw"))
            NODRAW = true; 

        std::string adrs = "127.0.0.1";
        if (GetParamsHolder().GetParamBool("ip"))
            adrs = GetParamsHolder().GetParam<std::string>("ip");

        if (GetParamsHolder().GetParamBool("name"))
            Debug::SetUniqueName(GetParamsHolder().GetParam<std::string>("name"));

        Manager man(adrs);
        man.initWorld();
        if (GetParamsHolder().GetParamBool("-auto"))
            man.ToogleAutoplay();
        man.process();
        return 0;
    }
    MapEditor map_editor;
    map_editor.InitWorld();
    map_editor.Run();
    return 0; 
}
