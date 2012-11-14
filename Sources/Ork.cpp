#include "Ork.h"
//#include "MonstrClass.h"
#include "MapClass.h"

#include "LiquidHolder.h"

#include "Turf.h"

//itemAdder<COrk> adder; 

COrk::COrk()
{
    tickSpeed = 1;
    pixSpeed = 1;
    SetSprite("icons/man.png");
    passable = true;
    v_level = 10;
    in_hand = 0;
    inside->addLiquid(hash("liquidblood"), 200);
    name = "Ork";
};

void COrk::aaMind()
{
};

void COrk::live()
{
    CAliveMob::live();
    auto li = owner->GetItem<Pit>();
    if (!li.valid())
        return;
    if (li->lhold->amountOfAll() >= li->lhold->size - 10)
        oxyless++;
}

void COrk::processGUImsg(const Message& msg)
{
    CAliveMob::processGUImsg(msg);
    if(msg.text == "SDLK_p")
    {
        if(in_hand.ret_id() != 0)
        {
            /*in_hand->posx = posx;
            in_hand->posy = posy;
            map->addItemOnMap(in_hand, true);
            in_hand = 0;*/
        }
        else
        {
            /*auto loc = map->getItem<SmallItem>(posx, posy);
            if(loc != map->squares[posx][posy].end())
            {
                in_hand = *loc;
                map->squares[posx][posy].erase(loc);
            }*/
        }
    }
    else if(msg.text == "SDLK_q")
    {
        IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("spear"), owner);
    }
    else if(msg.text == "SDLK_e")
    {
        IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("forcespear"), owner);
    }
    else if(msg.text == "SDLK_a")
    {
        /*if(in_hand.ret_id())
        {
            (*--(map->squares[posx - 1][posy].end()))->ignite(10);
            (*--(map->squares[posx - 1][posy].end()))->attack_by(in_hand);
        }*/
    }
    else if(msg.text == "SDLK_w")
    {
        /*if(in_hand.ret_id())
        {
            (*--(map->squares[posx][posy - 1].end()))->ignite(10);
            (*--(map->squares[posx][posy - 1].end()))->attack_by(in_hand);
        }*/
    }
    else if(msg.text == "SDLK_d")
    {
        /*if(in_hand.ret_id())
        {
            (*--(map->squares[posx + 1][posy].end()))->ignite(10);
            (*--(map->squares[posx + 1][posy].end()))->attack_by(in_hand);
        }*/
    }
    else if(msg.text == "SDLK_s")
    {
        /*if(in_hand.ret_id())
        {
            (*--(map->squares[posx][posy + 1].end()))->ignite(10);
            (*--(map->squares[posx][posy + 1].end()))->attack_by(in_hand);
        }*/
    }
};

void COrk::attack_by(id_ptr_on<SmallItem> atk, int force)
{
    SYSTEM_STREAM << "void COrk::attack_by(id_ptr_on<SmallItem> atk, int force)\n";
    //dmg++;
    CAliveMob::attack_by(atk, force);
}

/*
SAVED( int something )
cout << something;




*/