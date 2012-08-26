#ifndef SPRITE_H
#define SPRITE_H

#include <SDL.h>
#include <SDL_image.h>
#include "constheader.h"

void SetMasks(Uint32* rmask, Uint32* gmask, Uint32* bmask, Uint32* amask);

struct InitSprite
{
    std::string imgFile;
    int numFrameH;
    int numFrameW;
};

class CSprite
{
public:
    CSprite();
    int h;
    int w;
    int numFrameH;
    int numFrameW;
    SDL_Surface** frames;
    bool init(InitSprite data);
    ~CSprite();
};

#endif