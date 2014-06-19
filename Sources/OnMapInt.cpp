#include <assert.h>

#include "OnMapInt.h"
#include "MapClass.h"
#include "MobInt.h"
#include "Mob.h"
#include "MoveEffect.h"
#include "TileInt.h"

#include "mob_position.h"

void IOnMapItem::attack_by(id_ptr_on<SmallItem> it, int force) {};

std::list<HashAmount> IOnMapItem::insertLiquid(std::list<HashAmount> r) {return r;};

bool IOnMapItem::checkMove(Dir direct)
{
    if (!checkMoveTime()) 
        return false;
    dMove = direct;
    if (!checkPassable()) 
        return false;
    return mainMove();    
};

void IOnMapItem::move(Dir direct)
{
    assert(false);
    // TODO: remove
};

void IOnMapItem::SetSprite(const std::string& name)
{
    if (!GetMapMaster()) 
        return;
    sprite_ = GetSpriter()->returnSpr(name);
    T_SPR = name;
};


const GLSprite* IOnMapItem::GetSprite()
{
    if (sprite_ == nullptr)
        SetSprite(T_SPR);
    return sprite_;
}

void IOnMapItem::SetSpriteTop(const std::string& name)
{
    if (!GetMapMaster()) 
        return;
    sprite_top_ = GetSpriter()->returnSpr(name);
    T_SPR_TOP = name;
};


const GLSprite* IOnMapItem::GetSpriteTop()
{
    if (sprite_top_ == nullptr)
        SetSpriteTop(T_SPR_TOP);
    return sprite_top_;
}

const GLSprite* top_overlay = nullptr;
void IOnMapItem::processImage(DrawType type)
{ 
    if (NODRAW)
        return;

    const GLSprite* spr = GetSprite();
    bool is_need_overlay = false;
    if (type == TOP)
    {
        spr = GetSpriteTop();
        if (spr->Fail())
        {
            is_need_overlay = true;
            spr = GetSprite();
        }
    } 
    GetScreen()->Draw(spr, 
                      GetDrawX() + mob_position::get_shift_x(), 
                      GetDrawY() + mob_position::get_shift_y(), 
                      imageStateW, imageStateH);
    if (is_need_overlay)
    {
        if (!top_overlay)
                top_overlay = GetSpriter()->returnSpr("icons/top_overlay.png");
        GetScreen()->Draw(top_overlay, 
                          GetDrawX() + mob_position::get_shift_x(), 
                          GetDrawY() + mob_position::get_shift_y(), 
                          0, 0);
    }
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
bool IOnMapItem::isVisible(int x, int y)
{
    if ( x >= std::max(0, (castTo<CubeTile>(GetManager()->thisMob->GetOwner().ret_item())->posx()) - sizeHsq) &&
         x <= std::min((castTo<CubeTile>(GetManager()->thisMob->GetOwner().ret_item())->posx()) + sizeHsq, sizeHmap - 1) &&
         y >= std::max(0, (castTo<CubeTile>(GetManager()->thisMob->GetOwner().ret_item())->posy()) - sizeWsq) &&
         y <= std::min((castTo<CubeTile>(GetManager()->thisMob->GetOwner().ret_item())->posx()) + sizeWsq, sizeWmap - 1))
        return 1;
    return 0;
};

bool IOnMapItem::checkMoveTime()
{
    if(MAIN_TICK - lastMove < tickSpeed) 
        return false;
    lastMove = static_cast<int>(MAIN_TICK);
    return true;
};

// TODO: ÒÎÐÍÀÄÎ
bool IOnMapItem::checkPassable()
{
    return owner->GetNeighbour(dMove)->IsPassable();
};

bool IOnMapItem::mainMove()
{
    auto new_owner = owner->GetNeighbour(dMove);
    if (new_owner == owner)
        return false;

    owner->RemoveItem(GetId());
    new_owner->AddItem(GetId());

    if(new_owner->IsVisibleByPlayer())
    {
        Move* eff = getEffectOf<Move>();
        eff->Init(TITLE_SIZE, dMove, pixSpeed, GetId(), true);
        eff->Start();
    }
    return true;
};

void IOnMapItem::delThis()
{
    IOnMapBase::delThis();
}

IOnMapItem::IOnMapItem()
{
    sprite_ = nullptr;
    sprite_top_ = nullptr;
    v_level = 0;
    imageStateH = 0;
    imageStateW = 0;
    step_x = 0;
    step_y = 0;
    lastMove = 0;
    tickSpeed = 1;
    pixSpeed = 1;
    dMove = D_UP;
    passable = true;
    transparent = true;
    burn_power = 0;
    name = "NONAMESHIT";
    T_SPR = "NULL";
    T_SPR_TOP = "NULL";
}