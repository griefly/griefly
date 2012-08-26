#pragma once

#include <Typelist.h>

#include "MainInt.h"

class IDraw: public IMainItem
{
public:
    DECLARE_GET_TYPE_ITEM(IDraw)
    DECLARE_SAVED(IDraw, IMainItem);
    IDraw();
    void SetSprite(std::string& T_SPR);
    void SetSprite(const char* name);
    const GLSprite* GetSprite();
    virtual void processImage(SDL_Surface* surface);//draw this item
    bool IsTransp(int x, int y);
    int KV_ON_LOAD(x, 0);
    int KV_ON_LOAD(y, 0);
    int KV_SAVEBLE(v_level);
    int KV_SAVEBLE(imageStateH);
    int KV_SAVEBLE(imageStateW);
private:
    std::string KV_SAVEBLE(T_SPR);
    GLSprite* KV_ON_LOAD(sprite_, nullptr);
};

ADD_TO_TYPELIST(IDraw);