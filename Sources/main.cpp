/*#pragma comment (lib, "Libs/SDLmain.lib")
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
*/

#include <iostream>

#include <QApplication>
#include <QDesktopWidget>

#include "mainform.h"

#include "Mob.h"
#include "Params.h"

int main(int argc, char *argv[])
{   
    GetParamsHolder().ParseParams(argc, argv);
    QApplication app(argc, argv);
    SetQApp(&app);
    MainForm main_form;
    main_form.show();

    return app.exec();
}
