#include "Ork.h"
//#include "MonstrClass.h"
#include "MapClass.h"

#include "LiquidHolder.h"

#include "Turf.h"

//itemAdder<COrk> adder; 

COrk::COrk()
{
    tickSpeed = 2;
    pixSpeed = 1;
    SetSprite("icons/man.png");
    passable = true;
    v_level = 10;
    in_hand = 0;
    jump_time = 0;
    is_strong_owner = true;
    inside->addLiquid(hash("liquidblood"), 200);
    name = "Ork";
    
};

void COrk::aaMind()
{
};

void COrk::live()
{
    CAliveMob::live();
}
bool COrk::checkMove(Dir direct)
{
    if (CAliveMob::checkMove(direct))
    {   
        if (owner->GetItem<CWeed>().valid())
            PlaySound(&step_sound, "step.wav");
        return true;
    }
    return false;
}
void COrk::processGUImsg(const Message& msg)
{
    CAliveMob::processGUImsg(msg);
    if(msg.text == "SDLK_p")
    {
        if(in_hand.valid())
        {
            owner->AddItem(in_hand);
            in_hand = 0;
        }
        else
        {
            in_hand = owner->GetItem<SmallItem>();
            if (in_hand.valid())
            {
                if (!owner->RemoveItem(in_hand))
                {
                    SYSTEM_STREAM << "CANNOT DELETE ITEM WTF" << std::endl;
                }
                in_hand->SetOwner(id);
            }
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
    else if(msg.text == "SDLK_SPACE")
    {
        if ((MAIN_TICK - jump_time) > 20)
        {
            jump_time = MAIN_TICK;
            auto zup = owner->GetNeighbour(D_ZUP);
            if (zup.valid() && zup->IsPassable())
            {
                owner->RemoveItem(id);
                zup->AddItem(id);
                checkMove(dMove);
            }
            PlaySound(&jump_sound, "jump.ogx");
        }
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