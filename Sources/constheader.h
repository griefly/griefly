#ifndef CONSTHEADER_H
#define CONSTHEADER_H

#include <map>
#include <stdio.h>
#include <vector>
#include <SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

extern int DELAY_MAIN;//How often do process()

extern int FPS_MAX;//max fps

extern int DRAW_MAX; // max real fps

const int sizeHsq = 12; // visible size const
const int sizeWsq = 9;

const int sizeW = 640; // visible in pixel const
const int sizeH = 480;

const int sizeHmap = 50; // map size
const int sizeWmap = 50;
const int sizeDmap = 2;

const int beginMobPosX = 9; // posx and posx mob on screen
const int beginMobPosY = 7; // maybe no const?

extern bool NODRAW; // probably nodraw mode

extern int TITLE_SIZE;//no comment

extern std::ostream& SYSTEM_STREAM;//some system information here

const int HASH_OFTEN = 100;

typedef int Dir;
const Dir D_LEFT = 0;
const Dir D_RIGHT = 1;
const Dir D_UP = 2;
const Dir D_DOWN = 3;

struct VDir
{
    int x;
    int y;
    int z;
};

const VDir VD_LEFT = {-1, 0, 0};
const VDir VD_RIGHT = {1, 0, 0};
const VDir VD_UP = {0, -1, 0};
const VDir VD_DOWN = {0, 1, 0};

const VDir DirToVDir[4] = {VD_LEFT, VD_RIGHT, VD_UP, VD_DOWN};

const int MAX_LEVEL = 20;

extern unsigned long long MAIN_TICK;

const std::string RAW_DIR = "raw/";

#endif