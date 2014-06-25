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

const int sizeHsq = 13; // visible size const
const int sizeWsq = 10;

extern int sizeW; // visible in pixel const
extern int sizeH;

const int beginMobPosX = 9; // posx and posx mob on screen
const int beginMobPosY = 7; // maybe no const?

extern bool NODRAW; // probably nodraw mode

extern int TITLE_SIZE;//no comment

extern std::ostream& SYSTEM_STREAM;//some system information here

const int HASH_OFTEN = 20;

typedef int Dir;
const Dir D_LEFT = 0;
const Dir D_RIGHT = 1;
const Dir D_UP = 2;
const Dir D_DOWN = 3;
const Dir D_ZUP = 4;
const Dir D_ZDOWN = 5;

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
const VDir VD_ZUP = {0, 0, 1};
const VDir VD_ZDOWN = {0, 0, -1};

const VDir DirToVDir[6] = {VD_LEFT, VD_RIGHT, VD_UP, VD_DOWN, VD_ZUP, VD_ZDOWN};

const int MAX_LEVEL = 20;

extern unsigned long long MAIN_TICK;

const std::string RAW_DIR = "raw/";

#endif