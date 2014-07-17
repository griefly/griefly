#include <assert.h>

#include "OnMapInt.h"
#include "MapClass.h"
#include "MobInt.h"
#include "Mob.h"
#include "MoveEffect.h"
#include "TileInt.h"

#include "Creator.h"
#include "mob_position.h"
#include "helpers.h"

std::list<HashAmount> IOnMapObject::insertLiquid(std::list<HashAmount> r) {return r;};

void IOnMapObject::SetSprite(const std::string& name)
{
    T_SPR = name;
    if (!GetSpriter())
        return;
    sprite_ = GetSpriter()->returnSpr(name);
    SetState(state_);
};


const GLSprite* IOnMapObject::GetSprite()
{
    if (sprite_ == nullptr)
        SetSprite(T_SPR);
    return sprite_;
}

void IOnMapObject::SetState(const std::string& name)
{
    state_ = name;
    if (!GetSprite())
        return;
    if (GetSprite()->Fail())
        return;
    if (!GetSprite()->GetSDLSprite()->metadata.IsValidState(state_))
        return;

    metadata_ = &GetSprite()->GetSDLSprite()->metadata.GetSpriteMetadata(state_);
    
    image_state_ = 0;
    last_frame_tick_ = SDL_GetTicks();
}

const ImageMetadata::SpriteMetadata* IOnMapObject::GetMetadata()
{  
    if (!metadata_)
        SetState(state_);
    return metadata_;
}

const int ANIMATION_MUL = 100;

void IOnMapObject::DrawMain(int shift, int x, int y)
{
    if (NODRAW)
        return;

    if (!GetSprite() || GetSprite()->Fail() || !GetMetadata())
        return;

    int current_frame = GetMetadata()->frames_sequence[image_state_];
    int current_frame_pos = GetMetadata()->first_frame_pos + current_frame * GetMetadata()->dirs + shift;

    int image_state_h_ = current_frame_pos / GetSprite()->FrameW();
    int image_state_w_ = current_frame_pos % GetSprite()->FrameW();

    GetScreen()->Draw(GetSprite(), 
                      x, y, 
                      image_state_w_, image_state_h_);

    if (GetMetadata()->frames_sequence.size() == 1)
        return;
    if (GetMetadata()->frames_sequence
            [(image_state_ + 1) % GetMetadata()->frames_sequence.size()]
                == -1)
                    return;

    int time_diff = SDL_GetTicks() - last_frame_tick_;
    
    int next_state = image_state_;
    while (true)
    {
        // TODO: lags when time_diff very big
        int frame = GetMetadata()->frames_sequence[next_state];
        time_diff -= GetMetadata()->frames_data[frame].delay * ANIMATION_MUL;
        if (time_diff <= 0)
        {
            if (image_state_ == next_state)
                break;
            image_state_ = next_state;
            last_frame_tick_ = SDL_GetTicks();
            break;
        }
        next_state = (next_state + 1) % GetMetadata()->frames_sequence.size();
    }
}

void IOnMapObject::processImage(DrawType type)
{ 
    DrawMain(0, 
            GetDrawX() + mob_position::get_shift_x(),
            GetDrawY() + mob_position::get_shift_y());
};

void IOnMapObject::processPhysics()
{
    if (owner->IsStrongOwner())
        return;
    auto down = owner->GetNeighbour(D_ZDOWN);
    if (down.valid() && down->IsPassable(D_ZUP) && down->IsPassable(D_ALL))
    {
        owner->RemoveItem(GetId());
        down->AddItem(GetId());
    }
}

bool IOnMapObject::IsTransp(int x, int y)
{
    if (NODRAW)
        return true;
    if (!GetMetadata())
        return true;
    const CSprite* loc = GetSprite()->GetSDLSprite();
    if (y >= loc->h || x >= loc->w || x < 0 || y < 0)
        return true;

    int current_frame = GetMetadata()->frames_sequence[image_state_];
    int current_frame_pos = GetMetadata()->first_frame_pos + current_frame * GetMetadata()->dirs;

    int image_state_h_ = current_frame_pos / GetSprite()->FrameW();
    int image_state_w_ = current_frame_pos % GetSprite()->FrameW();

    SDL_Surface* surf = loc->frames[image_state_w_ * loc->numFrameH + image_state_h_];

    auto bpp = surf->format->BytesPerPixel;

    Uint8 un1, un2, un3, alpha;

    SDL_GetRGBA(static_cast<Uint32*>(surf->pixels)[y * surf->pitch / bpp + x], surf->format, &un1, &un2, &un3, &alpha);

    return alpha < 1;
}
bool IOnMapObject::isVisible(int x, int y)
{
    if ( x >= std::max(0, (castTo<CubeTile>(GetMob()->GetOwner().ret_item())->posx()) - sizeHsq) &&
         x <= std::min((castTo<CubeTile>(GetMob()->GetOwner().ret_item())->posx()) + sizeHsq, GetMapMaster()->GetMapH() - 1) &&
         y >= std::max(0, (castTo<CubeTile>(GetMob()->GetOwner().ret_item())->posy()) - sizeWsq) &&
         y <= std::min((castTo<CubeTile>(GetMob()->GetOwner().ret_item())->posx()) + sizeWsq, GetMapMaster()->GetMapW() - 1))
        return 1;
    return 0;
};

void IOnMapObject::delThis()
{
    IOnMapBase::delThis();
}

IOnMapObject::IOnMapObject()
{
    sprite_ = nullptr;
    v_level = 0;
    step_x = 0;
    step_y = 0;
    passable_all = true;
    passable_up = true;
    passable_down = true;
    passable_left = true;
    passable_right = true;
    transparent = true;
    burn_power = 0;
    name = "NONAMESHIT";
    T_SPR = "NULL";
    state_ = "null";
    
    image_state_ = -1;
    last_frame_tick_ = 0;
    metadata_ = nullptr;
}