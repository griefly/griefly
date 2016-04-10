#pragma once

#include <QImage>

#include "Metadata.h"

#include "core/Constheader.h"

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
    std::vector<QImage> frames;
    bool init(InitSprite data);
    ~CSprite();
};
