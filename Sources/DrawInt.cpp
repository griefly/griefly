#include "DrawInt.h"
#include "MapClass.h"

#include <SDL.h>

IDraw::IDraw()
{
    sprite_ = nullptr;
    v_level = 0;
    imageStateH = 0;
    imageStateW = 0;
    x = 0;
    y = 0;
    
    //
}

void IDraw::processImage(SDL_Surface* surface)
{ 
    if (NODRAW)
        return;
    mobMaster->gl_screen->Draw(GetSprite(), x, y, imageStateW, imageStateH);
};

bool IDraw::IsTransp(int x, int y)
{
    if (NODRAW)
        return false;
    const CSprite* loc = GetSprite()->GetSDLSprite();
    if (y >= loc->h || x >= loc->w || x < 0 || y < 0)
        return true;

    SDL_Surface* surf = loc->frames[imageStateW * loc->numFrameH + imageStateH];

    auto bpp = surf->format->BytesPerPixel;

    Uint8 un1, un2, un3, alpha;

    SDL_GetRGBA(static_cast<Uint32*>(surf->pixels)[y * surf->pitch / bpp + x], surf->format, &un1, &un2, &un3, &alpha);

    return alpha < 170;
}

void IDraw::SetSprite(std::string& name)
{
    if(!map) return;
    sprite_ = map->aSpr.returnSpr(name);
    T_SPR = name;
};

void IDraw::SetSprite(const char* name)
{
    //SYSTEM_STREAM << "Begin set sprite\n";
    if(!map) return;
    std::string namestr = name;
    sprite_ = map->aSpr.returnSpr(namestr);
    T_SPR = name;
    //SYSTEM_STREAM << "End set sprite\n";
};


const GLSprite* IDraw::GetSprite()
{
    if (sprite_ == nullptr)
        SetSprite(T_SPR);
    return sprite_;
}