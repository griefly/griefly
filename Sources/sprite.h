#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "Metadata.h"

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
    ImageMetadata metadata;
    int h;
    int w;
    int numFrameH;
    int numFrameW;
    SDL_Surface** frames;
    bool init(InitSprite data);
    ~CSprite();
};