#include "Ork.h"
#include "MapClass.h"

#include "LiquidHolder.h"
#include "Item.h"
#include "Shard.h"
#include "Turf.h"
#include "ItemFabric.h"
#include "visible_points.h"
#include "Door.h"
#include "sync_random.h"

COrk::COrk()
{
    tickSpeed = 1;
    pixSpeed = 2;
    SetSprite("icons/human.png");
    SetState("african1_m_s");
    SetPassable(D_ALL, true);
    v_level = 9;
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
        if (owner->GetItem<Shard>().valid())
            PlaySoundIfVisible("glass_step.ogg", GetOwner().ret_id());
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
            in_hand = owner->GetItem<Item>();
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
    else if (msg.text == "SDL_MOUSEBUTTONDOWN")
    {
        id_ptr_on<IOnMapObject> item = msg.from;
        if (item.valid() && item->GetOwner().valid())
        {
            SYSTEM_STREAM << "Item " << item->name << " clicked" << std::endl;
            // It isn't fine
            if (/*IsTileVisible(item->GetOwner().ret_id()) && */CanTouch(item, 1))
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

                id_ptr_on<Door> door = msg.from;
                if (door.valid())
                {
                    if (door->IsOpen())
                        door->Close();
                    else
                        door->Open();
                }
            }
        } 
    }

};