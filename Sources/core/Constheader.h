#pragma once

#include <map>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <SDL.h>

extern int DELAY_MAIN;//How often do process()

extern int FPS_MAX;//max fps

extern int DRAW_MAX; // max real fps

const int SIZE_H_SQ = 11; // visible size const
const int SIZE_W_SQ = 11;

extern int sizeW; // visible play in pixel const
extern int sizeH;

const int beginMobPosX = 8; // posx and posx mob on screen
const int beginMobPosY = 8; // maybe no const?

extern bool NODRAW; // probably nodraw mode

extern int TITLE_SIZE;//no comment

std::ostream& get_system_stream();

#define SYSTEM_STREAM get_system_stream()
//extern std::ostream& SYSTEM_STREAM;//some system information here

typedef int Dir;
const Dir D_ALL = -1;
const Dir D_LEFT = 0;
const Dir D_RIGHT = 1;
const Dir D_UP = 2;
const Dir D_DOWN = 3;
const Dir D_ZUP = 4;
const Dir D_ZDOWN = 5;

const Dir D_SOUTHEAST = 6;
const Dir D_SOUTHWEST = 7;
const Dir D_NORTHEAST = 8;
const Dir D_NORTHWEST = 9;

struct VDir
{
    int x;
    int y;
    int z;
};

struct point
{
    int posx;
    int posy;
    int posz;
};

inline bool NonZero(const VDir& vdir)
{
    return    vdir.x
           || vdir.y
           || vdir.z;
};

const VDir VD_LEFT = {-1, 0, 0};
const VDir VD_RIGHT = {1, 0, 0};
const VDir VD_UP = {0, -1, 0};
const VDir VD_DOWN = {0, 1, 0};
const VDir VD_ZUP = {0, 0, 1};
const VDir VD_ZDOWN = {0, 0, -1};

const VDir DirToVDir[6] = {VD_LEFT, VD_RIGHT, VD_UP, VD_DOWN, VD_ZUP, VD_ZDOWN};

inline Dir VDirToDir(const VDir& vdir)
{
    int abs_x = std::abs(vdir.x);
    int abs_y = std::abs(vdir.y);
    int abs_z = std::abs(vdir.z);
    if (abs_x > abs_y && abs_x > abs_z)
        if (vdir.x > 0)
            return D_RIGHT;
        else
            return D_LEFT;
    if (abs_y > abs_z)
        if (vdir.y > 0)
            return D_DOWN;
        else
            return D_UP;

    if (vdir.z > 0)
        return D_ZUP;
    return D_ZDOWN;
}

const Dir DirToRDir[6] = {1, 0, 3, 2, 5, 4};

const int MAX_LEVEL = 20;

extern size_t MAIN_TICK;

const std::string RAW_DIR = "raw/";

class Timer
{
public:
    void Start()
    {
        tick_ = SDL_GetTicks();
    }
    unsigned int Get()
    {
        return SDL_GetTicks() - tick_;
    }
private:
    unsigned int tick_;
};
