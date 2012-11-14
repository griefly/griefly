#include "Turf.h"
#include "MapClass.h"
#include "Liquid.h"

//itemAdder<CGround> adder;
//itemAdder<Pit> adder2;

std::list<HashAmount> Pit::insertLiquid(std::list<HashAmount> eh)
{
    return lhold->addLiquid(eh);
}

CGround::CGround()
{ 
    SetSprite("icons/turf.png");
    passable = false;
    transparent = false;
    v_level = 1;
    name = "Turf";
};

Pit::Pit()
{
    lhold = fabric->newItem<LiquidHolder>(id, hash("liquidholder"));
    SetSprite("icons/pit.png");
    passable = true;
    transparent = true;
    name = "Pit";
}

void Pit::processImage(SDL_Surface* s)
{
    IOnMapItem::processImage(s);
    lhold->processImage(s);
}

void Pit::process()
{
    /*lhold->master = id;
    if(!(map->checkOutBorder(posx, posy + 1) && map->checkOutBorder(posx + 1, posy) && map->checkOutBorder(posx - 1 , posy) && map->checkOutBorder(posx, posy - 1))) return;
    auto it1 = map->squares[posx + 1][posy].begin();

    if(Pit* l = castTo<Pit>(**it1))
        if(l->lhold->amountOfAll() < lhold->amountOfAll() - 1)
            l->lhold->addLiquid(lhold->removeLiquid((lhold->amountOfAll() - l->lhold->amountOfAll() + 1) / 2));

    it1 = map->squares[posx][posy + 1].begin();
    if(Pit* l = castTo<Pit>(**it1))
        if(l->lhold->amountOfAll() < lhold->amountOfAll() - 1)
            l->lhold->addLiquid(lhold->removeLiquid((lhold->amountOfAll() - l->lhold->amountOfAll() + 1) / 2));

    it1 = map->squares[posx - 1][posy].begin();
    if(Pit* l = castTo<Pit>(**it1))
        if(l->lhold->amountOfAll() < lhold->amountOfAll() - 1)
            l->lhold->addLiquid(lhold->removeLiquid((lhold->amountOfAll() - l->lhold->amountOfAll() + 1) / 2));

    it1 = map->squares[posx][posy - 1].begin();
    if(Pit* l = castTo<Pit>(**it1))
        if(l->lhold->amountOfAll() < lhold->amountOfAll() - 1)
            l->lhold->addLiquid(lhold->removeLiquid((lhold->amountOfAll() - l->lhold->amountOfAll() + 1) / 2));*/
}