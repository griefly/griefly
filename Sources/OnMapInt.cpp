#include <assert.h>

#include "OnMapInt.h"
#include "MapClass.h"
#include "MobInt.h"

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
    if (!map) 
        return;
    sprite_ = map->aSpr.returnSpr(name);
    T_SPR = name;
};


const GLSprite* IOnMapItem::GetSprite()
{
    if (sprite_ == nullptr)
        SetSprite(T_SPR);
    return sprite_;
}

void IOnMapItem::processImage(SDL_Surface* surface)
{ 
    if (NODRAW)
        return;
    mobMaster->gl_screen->Draw(GetSprite(), 
                               GetDrawX() + mob_position::get_shift_x(), 
                               GetDrawY() + mob_position::get_shift_y(), 
                               imageStateW, imageStateH);
};

void IOnMapItem::processPhysics()
{
    auto down = owner->GetNeighbour(D_ZDOWN);
    if (down.valid() && down->IsPassable())
    {
        owner->RemoveItem(id);
        down->AddItem(id);
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
    if ( x >= std::max(0, (castTo<CubeTile>(mobMaster->thisMob->GetOwner().ret_item())->posx()) - sizeHsq) &&
         x <= std::min((castTo<CubeTile>(mobMaster->thisMob->GetOwner().ret_item())->posx()) + sizeHsq, sizeHmap - 1) &&
         y >= std::max(0, (castTo<CubeTile>(mobMaster->thisMob->GetOwner().ret_item())->posy()) - sizeWsq) &&
         y <= std::min((castTo<CubeTile>(mobMaster->thisMob->GetOwner().ret_item())->posx()) + sizeWsq, sizeWmap - 1))
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

    owner->RemoveItem(id);
    new_owner->AddItem(id);

    if(new_owner->IsVisibleByPlayer())
    {
        Move* eff = getEffectOf<Move>();
        eff->Init(TITLE_SIZE, dMove, pixSpeed, id, true);
        eff->Start();
    }
    return true;
};

void IOnMapItem::delThis()
{
}

IOnMapItem::IOnMapItem()
{
    sprite_ = nullptr;
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
}