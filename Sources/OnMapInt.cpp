#include <assert.h>

#include "OnMapInt.h"
#include "MapClass.h"
#include "MobInt.h"
#include "Mob.h"
#include "MoveEffect.h"
#include "TileInt.h"

#include "Creator.h"
#include "mob_position.h"

void IOnMapItem::attack_by(id_ptr_on<SmallItem> it, int force) {};

std::list<HashAmount> IOnMapItem::insertLiquid(std::list<HashAmount> r) {return r;};

void IOnMapItem::SetSprite(const std::string& name)
{
    T_SPR = name;
    if (!GetSpriter())
        return;
    sprite_ = GetSpriter()->returnSpr(name);
    SetState(state_);
};


const GLSprite* IOnMapItem::GetSprite()
{
    if (sprite_ == nullptr)
        SetSprite(T_SPR);
    return sprite_;
}

void IOnMapItem::SetState(const std::string& name)
{
    state_ = name;
    if (!GetSprite()->GetSDLSprite()->metadata.IsValidState(state_))
    {
       // SYSTEM_STREAM << "State: " << name << " isn't valid for " << T_SPR << std::endl;
        return;
    }
    metadata_ = &GetSprite()->GetSDLSprite()->metadata.GetSpriteMetadata(state_);

    image_state_h_ = metadata_->first_frame_pos / GetSprite()->FrameW();
    image_state_w_ = metadata_->first_frame_pos % GetSprite()->FrameW();
}

const ImageMetadata::SpriteMetadata* IOnMapItem::GetMetadata()
{  
    if (!metadata_)
        SetState(state_);
    return metadata_;
}

int IOnMapItem::GetStateH()
{
    if (image_state_h_ == -1)
        SetState(state_);
    return image_state_h_;
}

int IOnMapItem::GetStateW()
{
    if (image_state_w_ == -1)
        SetState(state_);
    return image_state_w_;
}


void IOnMapItem::processImage(DrawType type)
{ 
    if (NODRAW)
        return;

    if (!GetSprite() || GetSprite()->Fail() || !GetMetadata())
        return;

    GetScreen()->Draw(GetSprite(), 
                      GetDrawX() + mob_position::get_shift_x(), 
                      GetDrawY() + mob_position::get_shift_y(), 
                      GetStateW(), GetStateH());
};

void IOnMapItem::processPhysics()
{
    if (owner->IsStrongOwner())
        return;
    auto down = owner->GetNeighbour(D_ZDOWN);
    if (down.valid() && down->IsPassable())
    {
        owner->RemoveItem(GetId());
        down->AddItem(GetId());
    }
}

bool IOnMapItem::IsTransp(int x, int y)
{
    if (NODRAW)
        return true;
    if (!GetMetadata())
        return true;
    const CSprite* loc = GetSprite()->GetSDLSprite();
    if (y >= loc->h || x >= loc->w || x < 0 || y < 0)
        return true;

    SDL_Surface* surf = loc->frames[GetStateW() * loc->numFrameH + GetStateH()];

    auto bpp = surf->format->BytesPerPixel;

    Uint8 un1, un2, un3, alpha;

    SDL_GetRGBA(static_cast<Uint32*>(surf->pixels)[y * surf->pitch / bpp + x], surf->format, &un1, &un2, &un3, &alpha);

    return alpha < 170;
}
bool IOnMapItem::isVisible(int x, int y)
{
    if ( x >= std::max(0, (castTo<CubeTile>(GetMob()->GetOwner().ret_item())->posx()) - sizeHsq) &&
         x <= std::min((castTo<CubeTile>(GetMob()->GetOwner().ret_item())->posx()) + sizeHsq, GetMapMaster()->GetMapH() - 1) &&
         y >= std::max(0, (castTo<CubeTile>(GetMob()->GetOwner().ret_item())->posy()) - sizeWsq) &&
         y <= std::min((castTo<CubeTile>(GetMob()->GetOwner().ret_item())->posx()) + sizeWsq, GetMapMaster()->GetMapW() - 1))
        return 1;
    return 0;
};

void IOnMapItem::delThis()
{
    IOnMapBase::delThis();
}

IOnMapItem::IOnMapItem()
{
    sprite_ = nullptr;
    v_level = 0;
    step_x = 0;
    step_y = 0;
    passable = true;
    transparent = true;
    burn_power = 0;
    name = "NONAMESHIT";
    T_SPR = "NULL";
    state_ = "null";
    image_state_h_ = -1;
    image_state_w_ = -1;
    metadata_ = nullptr;
}