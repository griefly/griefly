#pragma once

#include "EffectSystem.h"
#include "OnMapInt.h"

class Fire: public BaseEffect
{
public:
    id_ptr_on<IOnMapObject> master;
    GLSprite* sprite;
    int imageStateH;
    int imageStateW;
    int lastTicks;
    virtual void process() override;
    virtual void start() override;
    virtual void end() override;
    virtual void release() override;
    Fire();
};