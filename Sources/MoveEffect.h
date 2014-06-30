#pragma once

#include "EffectSystem.h"
#include "OnMapInt.h"

class Move: public BaseEffect
{
public:
    void Init(int way_n, Dir direct_n, int pixel_speed_n, 
              id_ptr_on<IOnMapItem> master_n, bool back_n = false);
    virtual void process() override;
    virtual void start() override;
    virtual void end() override;
    virtual void release() override;
    Move();
private:
    int pixel_speed;
    int way;
    Dir direct;
    bool back;
    id_ptr_on<IOnMapItem> master;
};