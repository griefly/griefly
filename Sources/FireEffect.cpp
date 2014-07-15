#include "FireEffect.h"

#include <assert.h>

#include "MapClass.h"

Fire::Fire()
{
    sprite = GetSpriter()->returnSpr("icons/fire_anim.png");
    master = 0;
}

void Fire::start()
{
    imageStateH = 0;
    imageStateW = 0;
    lastTicks = 0;
}

void Fire::process()
{
    assert(master.ret_id());
    //SYSTEM_STREAM << master.ret_id() << " " << this << "\n";
    if(master->burn_power <= 0)
    {
        Release();
        return;
    }
    const int DELAY = 60;
    if(SDL_GetTicks() - lastTicks > DELAY)
    {
        lastTicks = DELAY;
        imageStateW = (imageStateW + 1) % sprite->FrameW();
    }
    // TODO
    if(true/*master->master.ret_id() == 0 && IMainObject::mobMaster->isMobVisible(master->posx, master->posy)*/)
    {
     //   IMainObject::mobMaster->gl_screen->Draw(sprite, master->x, master->y, imageStateW, imageStateH);
    }
}

void Fire::end()
{
    SYSTEM_STREAM << "Fire::end()";
}

void Fire::release()
{
    SYSTEM_STREAM << "Fire::release()";
    master = 0;
}