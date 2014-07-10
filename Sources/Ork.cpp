#include "Ork.h"
#include "MapClass.h"

#include "LiquidHolder.h"
#include "CSmallItem.h"
#include "Turf.h"
#include "ItemFabric.h"

COrk::COrk()
{
    tickSpeed = 1;
    pixSpeed = 2;
    SetSprite("icons/human.png");
    SetState("african1_m_s");
    passable = true;
    v_level = 10;
    in_hand = 0;
    jump_time = 0;
    is_strong_owner = true;
    name = "Morgan James";
    
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
            PlaySound("glass_step.ogg");
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
                in_hand->SetOwner(GetId());
            }
        }
    }
    else if(msg.text == "SDLK_q")
    {
        GetItemFabric()->newItemOnMap<IOnMapItem>(hash("spear"), owner);
    }
    else if(msg.text == "SDLK_e")
    {
        GetItemFabric()->newItemOnMap<IOnMapItem>(hash("forcespear"), owner);
    }
    else if(msg.text == "SDLK_SPACE")
    {
        if ((MAIN_TICK - jump_time) > 20)
        {
            jump_time = static_cast<int>(MAIN_TICK);
            auto zup = owner->GetNeighbour(D_ZUP);
            if (zup.valid() && zup->IsPassable())
            {
                owner->RemoveItem(GetId());
                zup->AddItem(GetId());
                checkMove(dMove);
            }
          //  PlaySound("jump.ogx");
        }
    }
    else if (msg.text == "SDL_MOUSEBUTTONDOWN")
    {
        id_ptr_on<IOnMapItem> item = msg.from;
        if (item.valid())
        {
            SYSTEM_STREAM << "Item " << item->name << " clicked" << std::endl;
            if (CanTouch(item, 1))
            {
                SYSTEM_STREAM << "And we can touch it!" << std::endl;
                if(!in_hand.valid())
                {
                    in_hand = item;
                    if (in_hand.valid())
                    {
                        if (!item->GetOwner()->RemoveItem(item))
                        {
                            SYSTEM_STREAM << "CANNOT DELETE ITEM WTF" << std::endl;
                        }
                        item->SetOwner(GetId());
                    }
                    
                }
            }
        }
    }

};

void COrk::attack_by(id_ptr_on<SmallItem> atk, int force)
{
    SYSTEM_STREAM << "void COrk::attack_by(id_ptr_on<SmallItem> atk, int force)\n";
    //dmg++;
    CAliveMob::attack_by(atk, force);
}