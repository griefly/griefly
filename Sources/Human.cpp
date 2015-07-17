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
#include "Floor.h"

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

    lay_timer_ = 0;

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
    if (    !lying_
         && Friction::CombinedFriction(GetTurf())
        )
    {
        if (std::abs(force_.x) + std::abs(force_.y) + std::abs(force_.z) < 4)
        {
            if (msg.text == Input::MOVE_UP)
                ApplyForce(DirToVDir[D_UP]);
            else if (msg.text == Input::MOVE_DOWN)
                ApplyForce(DirToVDir[D_DOWN]);
            else if (msg.text == Input::MOVE_LEFT)
                ApplyForce(DirToVDir[D_LEFT]);
            else if (msg.text == Input::MOVE_RIGHT)
                ApplyForce(DirToVDir[D_RIGHT]);
        }
    }
    if (msg.type == Net::CHAT_TYPE)
    {
        //Chat::GetChat()->PostText(name + ": " + msg.text + "\n");
        Chat::GetChat()->PostWords(name, msg.text, owner->GetId());
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

            //SYSTEM_STREAM << "Item " << item->name << " clicked" << std::endl;
            // It isn't fine
            if (/*IsTileVisible(item->GetOwner().ret_id()) && */CanTouch(item, 1))
            {
                //SYSTEM_STREAM << "And we can touch it!" << std::endl;
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
                    {
                        interface_.Pick(0);
                        item->AttackBy(0);
                    }
                }
                else
                {
                    if (this->GetLying() == false)
                    {
                        item->AttackBy(interface_.GetActiveHand().Get());
                    }
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

void Human::SetLying(bool value)
{
    if (value == false && lay_timer_ > 0)
        return;

    lying_ = value;
    if (lying_)
    {
        Chat::GetChat()->PostSimpleText(name + " is lying now", owner->GetId());
        view_.SetAngle(90);
    }
    else
    {
        Chat::GetChat()->PostSimpleText(name + " is standing now!", owner->GetId());
        view_.SetAngle(0);
    }
    interface_.UpdateLaying();
}

void Human::AddLyingTimer(int value)
{
    lay_timer_ += value;
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
                Chat::GetChat()->PostSimpleText(name + " gasps!", owner->GetId());
        }
    }

    interface_.UpdateHealth();

    if (lay_timer_ > 0)
        --lay_timer_;

    if (health_ < 0)
    {
        if (!lying_)
        {
            SetLying(true);
        }
        if (health_ >= -100)
        {
            --health_;
            if (get_rand() % 5 == 0)
                Chat::GetChat()->PostSimpleText(name + " gasps!", owner->GetId());
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
    bool damaged = false;
    if (item && (item->damage > 0))
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
            Chat::GetChat()->PostDamage(item_owner->name, name, item->name, owner->GetId());
        }

        damaged = true;
    }
    else if (!item)
    {
        health_ -= 1;

        unsigned int punch_value = (get_rand() % 4) + 1;
        std::stringstream conv;
        conv << "punch" << punch_value << ".ogg";
        PlaySoundIfVisible(conv.str(), owner.ret_id());

        if (get_rand() % 5 == 0)
        {
            SetLying(true);
            AddLyingTimer(100);
            Chat::GetChat()->PostSimpleText(name + " has been knocked out!", owner->GetId());
        }

        damaged = true;
    }

    if (!damaged)
        return;

    if ((get_rand() % 3) != 0)
        return;

    unsigned int blood_value = (get_rand() % 7) + 1;
    std::stringstream conv;
    conv << "floor" << blood_value;

    if (id_ptr_on<Floor> f = GetTurf())
    {
        if (!f->bloody)
        {
            f->GetView()->AddOverlay("icons/blood.dmi", conv.str());
            f->bloody = true;
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
                 GetX(),
                 GetY(),
                 GetZ());
    }
}
