#include "Human.h"

#include "representation/Text.h"
#include "../ObjectFactory.h"
#include "../Names.h"
#include "representation/Sound.h"
#include "Shard.h"
#include "net/MagicStrings.h"
#include "representation/Chat.h"
#include "../atmos/AtmosHolder.h"
#include "Tile.h"
#include "../Helpers.h"
#include "../SyncRandom.h"
#include "Ghost.h"
#include "Clothes.h"
#include "Floor.h"
#include "net/NetworkMessagesTypes.h"
#include "Lobby.h"
#include "../Game.h"
#include "Gun.h"
#include "Weapons.h"
#include "ElectricTools.h"
#include "SpaceTurf.h"
#include "Drinks.h"
#include "MedbayTools.h"

Human::Human(quint32 id) : IMob(id)
{
    tick_speed_ = 1;
    SetSprite("icons/human.dmi");
    SetState("african1_m_s");
    SetPassable(D_ALL, Passable::BIG_ITEM);
    v_level = 9;
    attack_cooldown_ = 0;
    name = "Morgan James";
    passable_level = Passable::BIG_CREATURE;

    lay_timer_ = 0;

    dead_ = false;
    lying_ = false;
    max_health_ = 10000;
    suffocation_damage_ = 0;
    burn_damage_ = 0;
    brute_damage_ = 0;

    pulled_object_ = 0;
}

void Human::AfterWorldCreation()
{
    IMob::AfterWorldCreation();

    interface_.InitSlots();
    interface_.SetOwner(GetId());

    interface_.uniform_.Set(Create<Item>(JanitorUniform::T_ITEM_S()));
    interface_.feet_.Set(Create<Item>(OrangeBoots::T_ITEM_S()));
    interface_.r_hand_.Set(Create<Item>(Crowbar::T_ITEM_S()));

    interface_.uniform_.Get()->SetOwner(GetId());
    interface_.feet_.Get()->SetOwner(GetId());
    interface_.r_hand_.Get()->SetOwner(GetId());

    UpdateOverlays();

    name = GetGame().GetNames().GetMaleName();
}

void Human::InitGUI()
{
}

void Human::DeinitGUI()
{
}
void Human::GenerateInterfaceForFrame()
{
    IMob::GenerateInterfaceForFrame();
    interface_.Draw();
}

bool Human::TryMove(Dir direct)
{
    VDir pos;
    if (pulled_object_)
    {
        if (!CanTouch(pulled_object_))
        {
            StopPull();
        }
        else
        {
            pos.x = GetX() - pulled_object_->GetX();
            pos.y = GetY() - pulled_object_->GetY();
        }
    }
    if (IMob::TryMove(direct))
    {   
        if (owner->GetItem<Shard>().IsValid())
        {
            PlaySoundIfVisible("glass_step.ogg", GetOwner().Id());
        }

        TryClownBootsHonk();

        if (pulled_object_ && ((std::abs(pos.x) + std::abs(pos.y)) < 2))
        {
            if (!pulled_object_->TryMove(VDirToDir(pos)))
            {
                StopPull();
            }
        }
        return true;
    }
    return false;
}

void Human::ProcessMessage(const Message2 &msg)
{
    QJsonObject obj = Network2::ParseJson(msg);

    if (   msg.type == MessageType::ORDINARY
        && !lying_
        && Friction::CombinedFriction(GetTurf()))
    {
        if (qAbs(force_.x) + qAbs(force_.y) + qAbs(force_.z) < 4)
        {
            if (Network2::IsKey(obj, Input::MOVE_UP))
            {
                ApplyForce(DirToVDir[D_UP]);
                return;
            }
            else if (Network2::IsKey(obj, Input::MOVE_DOWN))
            {
                ApplyForce(DirToVDir[D_DOWN]);
                return;
            }
            else if (Network2::IsKey(obj, Input::MOVE_LEFT))
            {
                ApplyForce(DirToVDir[D_LEFT]);
                return;
            }
            else if (Network2::IsKey(obj, Input::MOVE_RIGHT))
            {
                ApplyForce(DirToVDir[D_RIGHT]);
                return;
            }
        }
    }
    if (msg.type == MessageType::MESSAGE)
    {
        QString text = obj["text"].toString();
        QString prefixes[] = {"me ", "me", "* ", "*"};
        bool found = false;
        for(auto& str : prefixes)
        {
            if (text.indexOf(str) == 0)
            {
                quint32 length = str.length();
                text.replace(0, length, name + " ");
                GetGame().GetChat().PostSimpleText(text, owner.Id());
                found = true;
                break;
            }
        }
        if (!found)
        {
            GetGame().GetChat().PostWords(name, text, owner.Id());
        }
    }
    else if (msg.type == MessageType::MOUSE_CLICK)
    {
        // TODO: shorter cd when shooting with weapons
        const int ATTACK_CD = 6;
        if ((MAIN_TICK - attack_cooldown_) < ATTACK_CD)
        {
            return;
        }
        attack_cooldown_ = MAIN_TICK;

        IdPtr<IOnMapObject> object = Network2::ExtractObjId(obj);
        if (!object.IsValid())
        {
            return;
        }

        QString action = Network2::ExtractAction(obj);
        if (action == Click::LEFT_SHIFT)
        {
            if (IdPtr<Human> human = object)
            {
                GetGame().GetChat().PostSimpleText(
                    name + " looks at " + human->name,
                    GetOwner().Id());
                return;
            }
            GetGame().GetChat().PostSimpleText(
                name + " looks at the " + object->name,
                GetOwner().Id());
            return;
        }
        if (action == Click::LEFT_CONTROL)
        {
            PullAction(object);
            return;
        }
        if (action == Click::LEFT_R)
        {
            RotationAction(object);
            return;
        }
        if (action != Click::LEFT)
        {
            qDebug() << "Unknown action: " << action;
            return;
        }
        if (lying_)
        {
            return;
        }

        if (IdPtr<IOnMapBase> object_owner = object->GetOwner())
        {
            if (CanTouch(object))
            {
                //SYSTEM_STREAM << "And we can touch it!" << std::endl;
                if(!interface_.GetActiveHand().Get())
                {
                    interface_.Pick(object);
                    if (interface_.GetActiveHand().Get())
                    {
                        if (!object_owner->RemoveItem(object))
                        {
                            qDebug() << "Unable to RemoveItem from owner during"
                                        " the pick phase!";
                            KvAbort();
                        }
                        object->SetOwner(GetId());
                    }
                    else
                    {
                        interface_.Pick(0);
                        object->AttackBy(0);
                    }
                }
                else
                {
                    if (GetLying() == false)
                    {
                        object->AttackBy(interface_.GetActiveHand().Get());
                    }
                }
                
            }
            else if (IdPtr<Gun> tool = interface_.GetActiveHand().Get())
            {
                if (GetLying() == false && Gun::Targetable(object))
                {
                    tool->Shoot(tool->TargetTileLoc(object));
                }
            }
            else if (  IdPtr<RemoteAtmosTool> tool
                     = interface_.GetActiveHand().Get())
            {
                IdPtr<CubeTile> tile = object->GetOwner();
                if (!tile)
                {
                    return;
                }
                GetGame().GetChat().PostTextFor(
                    AtmosTool::GetInfo(*tile->GetAtmosHolder()), GetId());
            }
        }
    }
    else
    {
        // TODO
        interface_.HandleClick(obj["key"].toString());
    }

}

void Human::UpdateOverlays()
{
    view_.RemoveOverlays();
    interface_.AddOverlays();
}

void Human::Process()
{
    IMob::Process();
    Live();
    if (pulled_object_)
    {
        if (!CanTouch(pulled_object_))
        {
            StopPull();
        }
    }
}

void Human::SetLying(bool value)
{
    if (value == false && lay_timer_ > 0)
    {
        return;
    }

    lying_ = value;
    if (lying_)
    {
        GetGame().GetChat().PostSimpleText(name + " is lying now", owner->GetId());
        view_.SetAngle(90);
        SetPassable(D_ALL, Passable::FULL);
        v_level = 8;
    }
    else
    {
        GetGame().GetChat().PostSimpleText(name + " is standing now!", owner->GetId());
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

    interface_.UpdateEnviroment();

    if (IdPtr<CubeTile> t = owner)
    {
        unsigned int oxygen = t->GetAtmosHolder()->GetGase(OXYGEN);
        int temperature = t->GetAtmosHolder()->GetTemperature();
        if(std::abs(40-temperature) > 3)
        {
            int damage = (std::abs(40-temperature) / 10) > 1 ? (std::abs(308-temperature) / 100) : 1;
            AddBurnDamage(damage);
        }
        if (oxygen > 0)
        {
            t->GetAtmosHolder()->RemoveGase(OXYGEN, 1);
            t->GetAtmosHolder()->AddGase(CO2, 1);
            Regeneration();
        }
        else if (CalculateHealth() >= -10000)
        {
            AddSuffocationDamage(100);
            
            if (GetRand() % 5 == 0 && ((MAIN_TICK % 3) == 0))
            {
                GetGame().GetChat().PostSimpleText(QString("%1 gasps!").arg(name), owner->GetId());
            }
        }
    }

    interface_.UpdateHealth();

    if (lay_timer_ > 0)
    {
        --lay_timer_;
    }

    if (CalculateHealth() < 0)
    {
        if (!lying_)
        {
            SetLying(true);
        }
        if (CalculateHealth() >= -10000)
        {
            AddSuffocationDamage(100);
            if (GetRand() % 4 == 0 && ((MAIN_TICK % 4) == 0))
            {
                GetGame().GetChat().PostSimpleText(QString("%1 gasps!").arg(name), owner->GetId());
            }
        }
    }
    if (CalculateHealth() < -10000 && !dead_)
    {
        OnDeath();
    }
}

void Human::Regeneration()
{
    int healed = suffocation_damage_ / 2 > 0 ? suffocation_damage_ / 2 : 1;
    if(suffocation_damage_ > 0)
    { 
        AddSuffocationDamage(-(healed));
    }
}

IdPtr<IOnMapBase> Human::GetNeighbour(Dir) const
{
    return GetId();
}

void Human::OnDeath()
{
    quint32 net_id = GetGame().GetFactory().GetNetId(GetId());
    if (net_id)
    {
        auto ghost = Create<Ghost>(Ghost::T_ITEM_S());
        ghost->name = name;
        GetGame().GetFactory().SetPlayerId(net_id, ghost.Id());
        owner->AddItem(ghost);
        if (GetId() == GetGame().GetMob().Id())
        {
            GetGame().ChangeMob(ghost);
        }
    }
    dead_ = true;
    SetFreq(0);

    GetLobby().security_score_ += 1;
}

void Human::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Drinks> drink = item)
    {
        drink->Drink(GetId(), item->GetOwner());
        return;
    }
    if(IdPtr<HealthAnalyzer> analyzer = item)
    {
        analyzer->Scan(GetId());
        return;
    }
    if(IdPtr<Medicine> medicine = item)
    {
        medicine->Heal(GetId());
        return;
    }
    bool damaged = false;
    if (item.IsValid() && (item->damage > 0))
    {
        AddBruteDamage(item->damage * 100);
        QString sound = QString("genhit%1.ogg").arg(GetRand() % 3 + 1);
        PlaySoundIfVisible(sound, owner.Id());
        if (IdPtr<IOnMapObject> item_owner = item->GetOwner())
        {
            GetGame().GetChat().PostDamage(item_owner->name, name, item->name, owner.Id());
        }

        damaged = true;
    }
    else if (!item.IsValid())
    {
        AddBruteDamage(100);

        unsigned int punch_value = (GetRand() % 4) + 1;
        PlaySoundIfVisible(QString("punch%1.ogg").arg(punch_value), owner.Id());

        if (GetRand() % 5 == 0)
        {
            SetLying(true);
            AddLyingTimer(100);
            GetGame().GetChat().PostSimpleText(name + " has been knocked out!", owner->GetId());
        }

        damaged = true;
    }

    if (!damaged)
    {
        return;
    }

    if ((GetRand() % 3) != 0)
    {
        return;
    }

    unsigned int blood_value = (GetRand() % 7) + 1;

    if (IdPtr<Floor> floor = GetTurf())
    {
        if (!floor->bloody)
        {
            floor->GetView()->AddOverlay(
                "icons/blood.dmi",
                QString("floor%1").arg(blood_value));
            floor->bloody = true;
        }
    }
}

void Human::Represent()
{
    Representation::Entity ent;
    ent.id = GetId();
    ent.pos_x = GetX();
    ent.pos_y = GetY();
    ent.vlevel = v_level;
    ent.view = *GetView();
    if (!lying_)
    {
        ent.dir = GetDir();
    }
    else
    {
        ent.dir = D_DOWN;
    }
    GetRepresentation().AddToNewFrame(ent);
}

void Human::CalculateVisible(std::list<PosPoint>* visible_list)
{
    if (CalculateHealth() >= 0)
    {
        GetGame().GetMap().CalculateVisisble(
            visible_list,
            GetX(),
            GetY(),
            GetZ());
    }
}

void Human::Bump(IdPtr<IMovable> item)
{
    if (IdPtr<Projectile> projectile = item)
    {
        AddBurnDamage(projectile->GetDamage() * 100);
        AddBurnDamage(projectile->GetBurnDamage() * 100);
        GetGame().GetChat().PostSimpleText(
            name + " got hit by a " + projectile->name + "!", GetRoot().Id());

        // TODO (?): sound

        if ((GetRand() % 3) != 0)
        {
            return;
        }

        unsigned int blood_value = (GetRand() % 7) + 1;

        if (IdPtr<Floor> floor = GetTurf()) // TODO Bleeding only with ballistic bullets
        {
            if (!floor->bloody)
            {
                floor->GetView()->AddOverlay(
                    "icons/blood.dmi",
                    QString("floor%1").arg(blood_value));
                floor->bloody = true;
            }
        }
        return;
    }
    IMovable::Bump(item);
}

void Human::RotationAction(IdPtr<IOnMapBase> item)
{
    if (IdPtr<IMovable> movable = item)
    {
        if (!movable->anchored_)
        {
            if (IdPtr<Projectile> projectile = movable)
            {
                return;
            }
            movable->Rotate((movable->GetDir() + 1) % 4);
        }
    }
}

void Human::PullAction(IdPtr<IOnMapBase> item)
{
    if (IdPtr<IMovable> movable = item)
    {
        if (CanTouch(movable))
        {
            if (IdPtr<Projectile> projectile = movable)
            {
                return;
            }
            pulled_object_ = movable;
            interface_.UpdatePulling(true);
        }
    }
}

void Human::StopPull()
{
    pulled_object_ = 0;
    interface_.UpdatePulling(false);
}

void Human::TryClownBootsHonk()
{
    if (lying_)
    {
        return;
    }
    if (MAIN_TICK % 3 != 0)
    {
        return;
    }
    IdPtr<ClownBoots> shoes = GetHumanInterface()->feet_.Get();
    if (!shoes.IsValid())
    {
        return;
    }
    if (IdPtr<Space> tile = GetTurf())
    {
        return;
    }
    QString sound = QString("clownstep%1.ogg").arg(GetRand() % 2 + 1);
    PlaySoundIfVisible(sound, GetOwner().Id());
}

int Human::CalculateHealth()
{
    return max_health_ - (suffocation_damage_ + burn_damage_ + brute_damage_);
}

CaucasianHuman::CaucasianHuman(quint32 id) : Human(id)
{
    SetState("caucasian2_m_s");
}

void Human::AddBurnDamage(int damage)
{
    burn_damage_ += damage;
}

void Human::AddSuffocationDamage(int damage)
{
    suffocation_damage_ += damage;
}

void Human::AddBruteDamage(int damage)
{
    brute_damage_ += damage;
}

void CaucasianHuman::AfterWorldCreation()
{
    // It is not mistake - we don't want to call Human function
    // because it creates some new items
    IMob::AfterWorldCreation();
    interface_.InitSlots();
    interface_.SetOwner(GetId());

    interface_.uniform_.Set(Create<Item>(RedUniform::T_ITEM_S()));
    interface_.feet_.Set(Create<Item>(OrangeBoots::T_ITEM_S()));
    interface_.r_hand_.Set(Create<Item>(Wrench::T_ITEM_S())); // wrench
    interface_.head_.Set(Create<Item>(Helmet::T_ITEM_S()));
    interface_.suit_.Set(Create<Item>(Armor::T_ITEM_S()));

    interface_.uniform_.Get()->SetOwner(GetId());
    interface_.feet_.Get()->SetOwner(GetId());
    interface_.r_hand_.Get()->SetOwner(GetId());
    interface_.head_.Get()->SetOwner(GetId());
    interface_.suit_.Get()->SetOwner(GetId());

    UpdateOverlays();

    name = GetGame().GetNames().GetMaleName();
}

void CaucasianHuman::OnDeath()
{
    Human::OnDeath();

    GetLobby().security_score_ -= 1;
    GetLobby().janitors_score_ += 1;
}
