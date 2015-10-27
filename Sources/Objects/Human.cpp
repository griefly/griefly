#include "Human.h"

#include "Text.h"
#include "ItemFabric.h"
#include "Names.h"
#include "sound.h"
#include "Shard.h"
#include "MagicStrings.h"
#include "Chat.h"
#include "AtmosHolder.h"
#include "Tile.h"
#include "helpers.h"
#include "mob_position.h"
#include "sync_random.h"
#include "Ghost.h"
#include "Creator.h"
#include "Clothes.h"
#include "Floor.h"
#include "NetworkMessagesTypes.h"
#include "Lobby.h"

Human::Human(size_t id) : IMob(id)
{
    tickSpeed = 1;
    pixSpeed = 2;
    SetSprite("icons/human.dmi");
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

void Human::AfterWorldCreation()
{
    IMob::AfterWorldCreation();

    interface_.uniform_.Set(GetItemFabric().newItem<Item>(JanitorUniform::T_ITEM_S()));
    interface_.feet_.Set(GetItemFabric().newItem<Item>(OrangeBoots::T_ITEM_S()));
    interface_.r_hand_.Set(GetItemFabric().newItem<Item>(Crowbar::T_ITEM_S()));

    interface_.uniform_.Get()->SetOwner(GetId());
    interface_.feet_.Get()->SetOwner(GetId());
    interface_.r_hand_.Get()->SetOwner(GetId());

    UpdateOverlays();
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

void Human::processGUImsg(const Message2 &msg)
{
    QJsonObject obj = Network2::ParseJson(msg);


 //   IMob::processGUImsg(msg);
    if (    msg.type == MessageType::ORDINARY
         && !lying_
         && Friction::CombinedFriction(GetTurf())
        )
    {
        if (std::abs(force_.x) + std::abs(force_.y) + std::abs(force_.z) < 4)
        {
            if (Network2::IsKey(obj, Input::MOVE_UP))
                ApplyForce(DirToVDir[D_UP]);
            else if (Network2::IsKey(obj, Input::MOVE_DOWN))
                ApplyForce(DirToVDir[D_DOWN]);
            else if (Network2::IsKey(obj, Input::MOVE_LEFT))
                ApplyForce(DirToVDir[D_LEFT]);
            else if (Network2::IsKey(obj, Input::MOVE_RIGHT))
                ApplyForce(DirToVDir[D_RIGHT]);
        }
    }
    if (msg.type == MessageType::MESSAGE)
    {
        std::string text = obj["text"].toString().toStdString();
        //GetChat().PostText(name + ": " + msg.text + "\n");
        if (Chat::IsOOCMessage(text))
        {
            GetChat().PostOOCText(name, text.substr(3));
        }
        else
        {
            GetChat().PostWords(name, text, owner.ret_id());
        }
    }
    else if (msg.type == MessageType::MOUSE_CLICK)
    {
        id_ptr_on<IOnMapObject> item = Network2::ExtractObjId(obj);
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
                    if (GetLying() == false)
                    {
                        item->AttackBy(interface_.GetActiveHand().Get());
                    }
                }
                
            }
        } 
    }
    else
    {
        // TODO
        interface_.HandleClick(obj["key"].toString().toStdString());
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
        GetChat().PostSimpleText(name + " is lying now", owner->GetId());
        view_.SetAngle(90);
        SetPassable(D_ALL, Passable::FULL);
        v_level = 8;
    }
    else
    {
        GetChat().PostSimpleText(name + " is standing now!", owner->GetId());
        view_.SetAngle(0);
        SetPassable(D_ALL, Passable::BIG_ITEM);
        v_level = 9;
    }
    interface_.UpdateLaying();
}

void Human::AddLyingTimer(int value)
{
    lay_timer_ += value;
}

void Human::Live()
{
    if (dead_)
    {
        return;
    }

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
            
            if (get_rand() % 5 == 0 && ((MAIN_TICK % 3) == 0))
                GetChat().PostSimpleText(name + " gasps!", owner->GetId());
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
            if (get_rand() % 4 == 0 && ((MAIN_TICK % 4) == 0))
                GetChat().PostSimpleText(name + " gasps!", owner->GetId());
        }
    }
    if (health_ < -100 && !dead_)
    {
        OnDeath();
    }
}

void Human::OnDeath()
{
    size_t net_id = GetItemFabric().GetNetId(GetId());
    if (net_id)
    {
        auto ghost = GetItemFabric().newItem<Ghost>(Ghost::T_ITEM_S());
        ghost->name = name;
        GetItemFabric().SetPlayerId(net_id, ghost.ret_id());
        owner->AddItem(ghost);
        if (GetId() == GetMob().ret_id())
        {
            ChangeMob(ghost);
        }
    }
    dead_ = true;
    SetFreq(0);

    GetLobby().security_score_ += 1;
}

void Human::AttackBy(id_ptr_on<Item> item)
{
    bool damaged = false;
    if (item.valid() && (item->damage > 0))
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
            GetChat().PostDamage(item_owner->name, name, item->name, owner.ret_id());
        }

        damaged = true;
    }
    else if (!item.valid())
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
            GetChat().PostSimpleText(name + " has been knocked out!", owner->GetId());
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
        DrawMain(helpers::dir_to_byond(GetDir()),
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


CaucasianHuman::CaucasianHuman(size_t id) : Human(id)
{
    SetState("caucasian2_m_s");
}

void CaucasianHuman::AfterWorldCreation()
{
    // It is not mistake - we don't want to call Human function
    // because it create some new items
    IMob::AfterWorldCreation();

    interface_.uniform_.Set(GetItemFabric().newItem<Item>(RedUniform::T_ITEM_S()));
    interface_.feet_.Set(GetItemFabric().newItem<Item>(OrangeBoots::T_ITEM_S()));
    interface_.r_hand_.Set(GetItemFabric().newItem<Item>(Wrench::T_ITEM_S()));
    interface_.head_.Set(GetItemFabric().newItem<Item>(Helmet::T_ITEM_S()));
    interface_.suit_.Set(GetItemFabric().newItem<Item>(Armor::T_ITEM_S()));

    interface_.uniform_.Get()->SetOwner(GetId());
    interface_.feet_.Get()->SetOwner(GetId());
    interface_.r_hand_.Get()->SetOwner(GetId());
    interface_.head_.Get()->SetOwner(GetId());
    interface_.suit_.Get()->SetOwner(GetId());

    UpdateOverlays();
}

void CaucasianHuman::OnDeath()
{
    Human::OnDeath();

    GetLobby().security_score_ -= 1;
    GetLobby().janitors_score_ += 1;
}
