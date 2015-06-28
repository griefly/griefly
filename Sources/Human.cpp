#include "Human.h"

#include "Text.h"
#include "ItemFabric.h"
#include "Names.h"
#include "sound.h"
#include "Shard.h"
#include "MagicStrings.h"
#include "Chat.h"
#include "AtmosHolder.h"
#include "TileInt.h"
#include "helpers.h"
#include "mob_position.h"
#include "sync_random.h"
#include "Ghost.h"
#include "Creator.h"
#include "Clothes.h"

Human::Human(size_t id) : IMob(id)
{
    tickSpeed = 1;
    pixSpeed = 2;
    SetSprite("icons/human.png");
    SetState("african1_m_s");
    SetPassable(D_ALL, Passable::BIG_ITEM);
    v_level = 9;
    is_strong_owner = true;
    name = GetMaleName();
  //  name = "Yes, it is human.";
    interface_.InitSlots();
    interface_.SetOwner(GetId());

    dead_ = false;
    lying_ = false;
    health_ = 100;
}
void Human::InitGUI()
{
}



void Human::DeinitGUI()
{
    //GetTexts().Delete("Sync");
}
void Human::processGUI()
{
    ////
    IMob::processGUI();
    //////

    interface_.Draw();
}

bool Human::checkMove(Dir direct)
{
    if (IMob::checkMove(direct))
    {   
        if (owner->GetItem<Shard>().valid())
            PlaySoundIfVisible("glass_step.ogg", GetOwner().ret_id());
        return true;
    }
    return false;
}

void Human::processGUImsg(const Message& msg)
{
 //   IMob::processGUImsg(msg);
    if (!lying_)
    {
        if (msg.text == Input::MOVE_UP)
            checkMove(D_UP);
        else if (msg.text == Input::MOVE_DOWN)
            checkMove(D_DOWN);
        else if (msg.text == Input::MOVE_LEFT)
            checkMove(D_LEFT);
        else if (msg.text == Input::MOVE_RIGHT)
            checkMove(D_RIGHT);
    }
    if (msg.type == Net::CHAT_TYPE)
    {
        Chat::GetChat()->PostText(name + ": " + msg.text + "\n");
    }
    if(msg.text == Input::KEY_Q)
    {
        GetItemFabric()->newItemOnMap<Item>(Armor::T_ITEM_S(), GetOwner());
    }
    if(msg.text == Input::KEY_W)
    {
        GetItemFabric()->newItemOnMap<Item>(Helmet::T_ITEM_S(), GetOwner());
    }
    if(msg.text == Input::KEY_E)
    {
        GetItemFabric()->newItemOnMap<Item>(EngineUniform::T_ITEM_S(), GetOwner());
    }
    if(msg.text == Input::KEY_R)
    {
        GetItemFabric()->newItemOnMap<Item>(OrangeBoots::T_ITEM_S(), GetOwner());
    }
    if(msg.text == "SDLK_p")
    {
        if(interface_.GetRHand()) // TODO: active hand
        {
            owner->AddItem(interface_.GetRHand());
            interface_.Drop();
        }
    }
    else if(msg.text == "SDLK_z")
    {
        id_ptr_on<CubeTile> ct = GetOwner();
        ct->GetAtmosHolder()->AddGase(OXYGEN, 1000);
    }
    else if (msg.text == "SDLK_x")
    {
        id_ptr_on<CubeTile> ct = GetOwner();
        ct->GetAtmosHolder()->AddGase(CO2, 1000);
    }
    else if (msg.text == "SDLK_c")
    {
        id_ptr_on<CubeTile> ct = GetOwner();
        ct->GetAtmosHolder()->AddGase(NYTROGEN, 1000);
    }
    else if (msg.text == "SDLK_v")
    {
        id_ptr_on<CubeTile> ct = GetOwner();
        ct->GetAtmosHolder()->AddEnergy(10000);
    }
    else if (msg.text == "SDLK_s_down")
    {
        interface_.SwapHands();
    }
    else if (msg.text == Input::LEFT_CLICK)
    {
        id_ptr_on<IOnMapObject> item = msg.from;
        if (item && item->GetOwner())
        {

            SYSTEM_STREAM << "Item " << item->name << " clicked" << std::endl;
            // It isn't fine
            if (/*IsTileVisible(item->GetOwner().ret_id()) && */CanTouch(item, 1))
            {
                SYSTEM_STREAM << "And we can touch it!" << std::endl;
                if(!interface_.GetActiveHand().Get())
                {
                    interface_.Pick(item);
                    if (interface_.GetActiveHand().Get())
                    {
                        if (!item->GetOwner()->RemoveItem(item))
                            SYSTEM_STREAM << "CANNOT DELETE ITEM WTF" << std::endl;
                        item->SetOwner(GetId());
                    }
                    else
                        interface_.Pick(0);
                }
                else
                {
                    item->AttackBy(interface_.GetActiveHand().Get());
                }
                
            }
        } 
    }
    else
    {
        interface_.HandleClick(msg.text);
    }

}

void Human::UpdateOverlays()
{
    view_.RemoveOverlays();
    interface_.AddOverlays();
}

void Human::process()
{
    IMob::process();
    Live();
}

void Human::Live()
{
    if (id_ptr_on<CubeTile> t = owner)
    {
        unsigned int oxygen = t->GetAtmosHolder()->GetGase(OXYGEN);
        if (oxygen > 0)
        {
            t->GetAtmosHolder()->RemoveGase(OXYGEN, 1);
            t->GetAtmosHolder()->AddGase(CO2, 1);
        }
        else if (health_ >= -100)
        {
            --health_;
            
            if (get_rand() % 5 == 0)
                Chat::GetChat()->PostText(name + " gasps!\n");
        }
    }

    interface_.UpdateHealth();

    if (health_ < 0)
    {
        if (!lying_)
        {
            lying_ = true;
            view_.SetAngle(90);
        }
        if (health_ >= -100)
        {
            --health_;
            if (get_rand() % 5 == 0)
                Chat::GetChat()->PostText(name + " gasps!\n");
        }
    }
    if (health_ < -100 && !dead_)
    {
        size_t net_id = GetItemFabric()->GetNetId(GetId());
        if (net_id)
        {
            auto ghost = GetItemFabric()->newItem<Ghost>(Ghost::T_ITEM_S());
            GetItemFabric()->SetPlayerId(net_id, ghost.ret_id());
            owner->AddItem(ghost);
            if (thisMobControl)
            {
                ChangeMob(ghost);
            }
        }
        dead_ = true;
    }
}

void Human::AttackBy(id_ptr_on<Item> item)
{
    if (item->damage > 0)
    {
        health_ -= item->damage;
        unsigned int value = get_rand() % 3;
        std::string snd;
        if (value == 0)
            snd = "genhit1.ogg";
        if (value == 1)
            snd = "genhit2.ogg";
        if (value == 2)
            snd = "genhit3.ogg";
        PlaySoundIfVisible(snd, owner.ret_id());
        if (id_ptr_on<IOnMapObject> item_owner = item->GetOwner())
        {
            Chat::GetChat()->PostText(
                name + " is attacked by " + item_owner->name + " with " + item->name + "\n"
                );
        }
    }
}

void Human::processImage(DrawType type)
{
    if (NODRAW)
        return;
    if (!GetSprite() || GetSprite()->Fail() || !GetMetadata())
        return;

    if (GetMetadata()->dirs >= 4 && !lying_)
    {
        DrawMain(helpers::dir_to_byond(dMove),            
            GetDrawX() + mob_position::get_shift_x(),
            GetDrawY() + mob_position::get_shift_y());
    }
    else
    {
        DrawMain(0,            
            GetDrawX() + mob_position::get_shift_x(),
            GetDrawY() + mob_position::get_shift_y());
    }
}

void Human::CalculateVisible(std::list<point>* visible_list)
{
    if (health_ >= 0)
    {
        visible_list = 
            GetMapMaster()->losf.calculateVisisble(visible_list, 
                GetMob()->GetX(), 
                GetMob()->GetY(),
                GetMob()->GetZ());
    }
}
