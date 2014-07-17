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
#include "ASpritesClass.h"

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
    gui_sprite_ = nullptr;
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

void COrk::processGUI()
{
    CAliveMob::processGUI();

    if (!gui_sprite_)
        gui_sprite_ = GetSpriter()->returnSpr("icons/screen_retro.dmi");
    // TODO:

    // Lhand
    GetScreen()->Draw(gui_sprite_, 
                      0  * 32, 
                      14 * 32, 
                      1, 3);

    // Rhand
    GetScreen()->Draw(gui_sprite_, 
                      2  * 32, 
                      14 * 32, 
                      2, 3);

    // Body
    GetScreen()->Draw(gui_sprite_, 
                      1  * 32, 
                      14 * 32, 
                      1, 0);

    // Helmet
    GetScreen()->Draw(gui_sprite_, 
                      1  * 32, 
                      13 * 32, 
                      0, 2);
    if (in_hand)
        in_hand->DrawMain(0, 0 * 32, 14 * 32);
}

void COrk::processGUImsg(const Message& msg)
{
    CAliveMob::processGUImsg(msg);
    if(msg.text == "SDLK_p")
    {
        if(in_hand)
        {
            owner->AddItem(in_hand);
            in_hand = 0;
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