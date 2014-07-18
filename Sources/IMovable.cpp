#include "IMovable.h"

#include "EffectSystem.h"
#include "MoveEffect.h"
#include "helpers.h"
#include "MobInt.h"
#include "mob_position.h"

#include <assert.h>

IMovable::IMovable()
{
    lastMove = 0;
    tickSpeed = 1;
    pixSpeed = 1;
    dMove = D_UP;
    anchored = false;
}

bool IMovable::checkMove(Dir direct)
{
    if (!checkMoveTime()) 
        return false;
    if (anchored)
        return false;
    dMove = direct;
    if (!checkPassable())
    {
        return false;
    }
    return mainMove();    
};

void IMovable::move(Dir direct)
{
    assert(false);
    // TODO: remove
};

bool IMovable::checkMoveTime()
{
    if((static_cast<int>(MAIN_TICK) - lastMove) < tickSpeed) 
        return false;
    lastMove = static_cast<int>(MAIN_TICK);
    return true;
};

// TODO: ÒÎÐÍÀÄÎ
bool IMovable::checkPassable()
{
    if (!owner->IsPassable(dMove))
    {
        owner->Bump(GetId());
        return false;
    }
    if (   !owner->GetNeighbour(dMove)->IsPassable(D_ALL)
        || !owner->GetNeighbour(dMove)->IsPassable(helpers::revert_dir(dMove)))
    {
        owner->GetNeighbour(dMove)->Bump(GetId());
        return false;
    }
    
    return true;
};

bool IMovable::mainMove()
{
    auto new_owner = owner->GetNeighbour(dMove);
    if (new_owner == owner)
        return false;

    owner->RemoveItem(GetId());
    new_owner->AddItem(GetId());

    if(new_owner->IsVisibleByPlayer())
    {
        Move* eff = EffectFabricOf<Move>::getEffectOf();
        eff->Init(TITLE_SIZE, dMove, pixSpeed, GetId(), true);
        eff->Start();
    }
    return true;
};

void IMovable::processImage(DrawType type)
{
    if (NODRAW)
        return;
    if (!GetSprite() || GetSprite()->Fail() || !GetMetadata())
        return;

    if (GetMetadata()->dirs >= 4)
    {
        DrawMain(helpers::dir_to_byond(dMove),            
            GetDrawX() + mob_position::get_shift_x(),
            GetDrawY() + mob_position::get_shift_y());
    }
    else
    {
        DrawMain(0,            
            GetDrawX() + mob_position::get_shift_x(),
            GetDrawY() + mob_position::get_shift_y());
    }
}

void IMovable::Bump(id_ptr_on<IMovable> item)
{
    id_ptr_on<IMob> m;
    m = item;
    if (m)
        checkMove(m->dMove);
}

bool IMovable::IsTransp(int x, int y)
{
    if (NODRAW)
        return true;
    if (!GetMetadata())
        return true;
    const CSprite* loc = GetSprite()->GetSDLSprite();
    if (y >= loc->h || x >= loc->w || x < 0 || y < 0)
        return true;

    int shift = 0;
    if (GetMetadata()->dirs >= 4)
        shift = helpers::dir_to_byond(dMove);
    int current_frame = GetMetadata()->frames_sequence[image_state_];
    int current_frame_pos = GetMetadata()->first_frame_pos + current_frame * GetMetadata()->dirs + shift;

    int image_state_h_ = current_frame_pos / GetSprite()->FrameW();
    int image_state_w_ = current_frame_pos % GetSprite()->FrameW();

    SDL_Surface* surf = loc->frames[image_state_w_ * loc->numFrameH + image_state_h_];

    auto bpp = surf->format->BytesPerPixel;

    Uint8 un1, un2, un3, alpha;

    SDL_GetRGBA(static_cast<Uint32*>(surf->pixels)[y * surf->pitch / bpp + x], surf->format, &un1, &un2, &un3, &alpha);

    return alpha < 1;
}