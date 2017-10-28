#include "Human.h"

#include "core/ObjectFactory.h"
#include "core/Names.h"
#include "representation/Sound.h"
#include "core/objects/movable/items/Shard.h"
#include "net/MagicStrings.h"
#include "core/atmos/AtmosHolder.h"
#include "core/objects/Tile.h"
#include "core/Helpers.h"
#include "core/SynchronizedRandom.h"
#include "Ghost.h"
#include "core/objects/movable/items/Clothes.h"
#include "core/objects/turfs/Floor.h"
#include "core/objects/Lobby.h"
#include "core/Game.h"
#include "core/objects/movable/items/Gun.h"
#include "core/objects/movable/items/Weapons.h"
#include "core/objects/movable/items/ElectricTools.h"
#include "core/objects/turfs/SpaceTurf.h"
#include "core/objects/movable/items/Drinks.h"
#include "core/objects/movable/items/MedbayTools.h"

#include "representation/Representation.h"

using namespace kv;

QVector<Position> Human::GetHeardPoints() const
{
    QVector<Position> retval;
    CalculateVisible(&retval);
    return retval;
}

void Human::Hear(const Phrase& phrase)
{
    if (IsMinded())
    {
        GetGame().GetChatFrameInfo().PostPersonal(
            QString("<b>%1</b> <i>says</i>, <span>\"%2\"</span>").arg(phrase.from).arg(phrase.text),
            GetGame().GetNetId(GetId()));
    }
}

Human::Human()
{
    tick_speed_ = 1;
    SetSprite("icons/human.dmi");
    SetState("african1_m_s");
    SetPassable(Dir::ALL, passable::BIG_ITEM);
    v_level = 9;
    attack_cooldown_ = 0;
    name = "Morgan James";
    passable_level = passable::BIG_CREATURE;

    lay_timer_ = 0;

    dead_ = false;
    lying_ = false;
    max_health_ = HUMAN_MAX_HEALTH;
    suffocation_damage_ = 0;
    burn_damage_ = 0;
    brute_damage_ = 0;

    pulled_object_ = 0;
}

void Human::AfterWorldCreation()
{
    Mob::AfterWorldCreation();

    interface_ = Create<HumanInterface>(HumanInterface::GetTypeStatic());
    interface_->SetOwner(GetId());

    UpdateOverlays();

    name = GetGame().GetNames().GetMaleName();
}

void Human::MindEnter()
{
    // Nothing
}

void Human::MindExit()
{
    // Nothing
}

void Human::GenerateInterfaceForFrame(GrowingFrame* representation)
{
    Mob::GenerateInterfaceForFrame(representation);
    interface_->Represent(representation);
}

bool Human::TryMove(Dir direct)
{
    Vector pos;
    if (pulled_object_)
    {
        if (!CanTouch(pulled_object_))
        {
            StopPull();
        }
        else
        {
            pos.x = GetPosition().x - pulled_object_->GetPosition().x;
            pos.y = GetPosition().y - pulled_object_->GetPosition().y;
        }
    }
    if (Mob::TryMove(direct))
    {   
        if (GetOwner()->GetItem<Shard>().IsValid())
        {
            PlaySoundIfVisible("glass_step.wav");
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

void Human::ProcessMessage(const Message& message)
{
    if (   message.type == message_type::ORDINARY
        && !lying_
        && friction::CombinedFriction(GetTurf()))
    {
        if (qAbs(force_.x) + qAbs(force_.y) + qAbs(force_.z) < 4)
        {
            if (Network2::IsKey(message.data, Input::MOVE_UP))
            {
                ApplyForce(DirToVDir(Dir::NORTH));
                return;
            }
            else if (Network2::IsKey(message.data, Input::MOVE_DOWN))
            {
                ApplyForce(DirToVDir(Dir::SOUTH));
                return;
            }
            else if (Network2::IsKey(message.data, Input::MOVE_LEFT))
            {
                ApplyForce(DirToVDir(Dir::WEST));
                return;
            }
            else if (Network2::IsKey(message.data, Input::MOVE_RIGHT))
            {
                ApplyForce(DirToVDir(Dir::EAST));
                return;
            }
        }
    }
    if (message.type == message_type::MESSAGE)
    {
        QString text = message.data["text"].toString();
        const QString prefixes[] = {"me ", "me", "* ", "*"};
        bool found = false;
        for (auto& str : prefixes)
        {
            if (text.startsWith(str))
            {
                quint32 length = str.length();
                text.replace(0, length, "");
                MakeEmote(text);
                found = true;
                break;
            }
        }
        if (!found)
        {
            Phrase phrase;
            phrase.from = name.toHtmlEscaped();
            phrase.text = text.toHtmlEscaped();
            if (!phrase.text.isEmpty())
            {
                phrase.text[0] = phrase.text[0].toUpper();
            }
            GetGame().GetChatFrameInfo().PostHear(phrase, GetPosition());
        }
    }
    else if (message.type == message_type::MOUSE_CLICK)
    {
        // TODO: shorter cd when shooting with weapons
        const int ATTACK_CD = 6;
        if ((GetGameTick() - attack_cooldown_) < ATTACK_CD)
        {
            return;
        }
        attack_cooldown_ = GetGameTick();

        IdPtr<MaterialObject> object = Network2::ExtractObjId(message.data);
        if (!object.IsValid())
        {
            return;
        }

        QString action = Network2::ExtractAction(message.data);
        if (action == Click::LEFT_SHIFT)
        {
            if (IdPtr<Human> human = object)
            {
                PostVisible(
                    name + " looks at " + human->name,
                    GetPosition());
                return;
            }
            PostVisible(
                name + " looks at the " + object->name,
                GetPosition());
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

        if (IdPtr<MapObject> object_owner = object->GetOwner())
        {
            if (CanTouch(object))
            {
                if (!interface_->GetItemInActiveHand().IsValid())
                {
                    interface_->PickItem(object);
                    if (interface_->GetItemInActiveHand().IsValid())
                    {
                        if (!object_owner->RemoveObject(object))
                        {
                            kv::Abort("Unable to RemoveItem from GetOwner() during"
                                    " the pick phase!");
                        }
                        object->SetOwner(GetId());
                    }
                    else
                    {
                        interface_->DropItem();
                        object->AttackBy(0);
                    }
                }
                else
                {
                    if (GetLying() == false)
                    {
                        object->AttackBy(interface_->GetItemInActiveHand());
                    }
                }
                
            }
            else if (IdPtr<Gun> tool = interface_->GetItemInActiveHand())
            {
                if (GetLying() == false && Gun::Targetable(object))
                {
                    tool->Shoot(tool->TargetTileLoc(object));
                }
            }
            else if (  IdPtr<RemoteAtmosTool> tool
                     = interface_->GetItemInActiveHand())
            {
                IdPtr<CubeTile> tile = object->GetOwner();
                if (!tile)
                {
                    return;
                }
                PostHtmlFor(AtmosTool::GetHtmlInfo(*tile->GetAtmosHolder()), GetId());
            }
        }
    }
    else
    {
        // TODO
        interface_->HandleClick(message.data["key"].toString());
    }

}

void Human::UpdateOverlays()
{
    view_.RemoveOverlays();
    interface_->AddOverlays(&view_);
}

void Human::Process()
{
    Mob::Process();
    Live();
    if (pulled_object_)
    {
        if (!CanTouch(pulled_object_))
        {
            StopPull();
        }
    }
}

void Human::SetLaying(bool value)
{
    if (value == false && lay_timer_ > 0)
    {
        return;
    }

    lying_ = value;
    if (lying_)
    {
        PostVisible(name + " is lying now", GetPosition());
        view_.SetAngle(90);
        SetPassable(Dir::ALL, passable::FULL);
        v_level = 8;
    }
    else
    {
        PostVisible(name + " is standing now!", GetPosition());
        view_.SetAngle(0);
        SetPassable(Dir::ALL, passable::BIG_ITEM);
        v_level = 9;
    }
    interface_->UpdateLaying(lying_);
}

void Human::AddLayingTimer(int value)
{
    lay_timer_ += value;
}

void Human::Live()
{
    if (dead_)
    {
        return;
    }

    if (IdPtr<CubeTile> t = GetOwner())
    {
        const unsigned int oxygen = t->GetAtmosHolder()->GetGase(atmos::OXYGEN);
        const int temperature = t->GetAtmosHolder()->GetTemperature();
        const int pressure = t->GetAtmosHolder()->GetPressure();

        interface_->UpdateEnvironment(temperature, pressure, oxygen);

        const int BURNING_THRESHOLD = 3;
        const int MIN_BURN_DAMAGE = 1;
        if (qAbs(REGULAR_TEMPERATURE - temperature) > BURNING_THRESHOLD)
        {
            int damage = qMax(MIN_BURN_DAMAGE,qAbs(REGULAR_TEMPERATURE - temperature));
            ApplyBurnDamage(damage);
        }
        if (oxygen > 0)
        {
            t->GetAtmosHolder()->RemoveGase(atmos::OXYGEN, 1);
            t->GetAtmosHolder()->AddGase(atmos::CO2, 1);
            Regeneration();
        }
        else if (CalculateHealth() >= -1 * HUMAN_MAX_HEALTH)
        {
            ApplySuffocationDamage(100);
            
            if (GenerateRandom() % 5 == 0 && ((GetGameTick() % 3) == 0))
            {
                MakeEmote("gasps!");
            }
        }
    }

    interface_->UpdateHealth(CalculateHealth());

    if (lay_timer_ > 0)
    {
        --lay_timer_;
    }

    if (CalculateHealth() < 0)
    {
        if (!lying_)
        {
            SetLaying(true);
        }
        if (CalculateHealth() >= -1 * HUMAN_MAX_HEALTH)
        {
            ApplySuffocationDamage(100);
            if (GenerateRandom() % 4 == 0 && ((GetGameTick() % 4) == 0))
            {
                MakeEmote("gasps!");
            }
        }
    }
    if (CalculateHealth() < -1 * HUMAN_MAX_HEALTH && !dead_)
    {
        OnDeath();
    }
}

void Human::Regeneration()
{
    if (GetSuffocationDamage() <= 0)
    {
        return;
    }
    ApplySuffocationDamage(-1 * 50);
}

IdPtr<MapObject> Human::GetNeighbour(Dir) const
{
    return GetId();
}

void Human::OnDeath()
{
    if (IsMinded())
    {
        auto ghost = Create<Ghost>(Ghost::GetTypeStatic(), GetOwner());
        ghost->name = name;

        MoveMindTo(ghost);
    }
    dead_ = true;
    SetFreq(0);
}

void Human::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Drinks> drink = item)
    {
        drink->Drink(GetId(), item->GetOwner());
        return;
    }
    if (IdPtr<HealthAnalyzer> analyzer = item)
    {
        analyzer->Scan(GetId());
        return;
    }
    if (IdPtr<Medicine> medicine = item)
    {
        medicine->Heal(GetId());
        return;
    }
    bool damaged = false;
    if (item.IsValid() && (item->damage > 0))
    {
        ApplyBruteDamage(item->damage * 100);
        QString sound = QString("genhit%1.wav").arg(GenerateRandom() % 3 + 1);
        PlaySoundIfVisible(sound);
        if (IdPtr<MaterialObject> item_owner = item->GetOwner())
        {
            PostVisible(
                QString("<font color=\"red\">%1 is attacked by %2 with %3</font>")
                    .arg(name).arg(item_owner->name).arg(item->name),
                GetPosition());
        }

        damaged = true;
    }
    else if (!item.IsValid())
    {
        ApplyBruteDamage(100);

        unsigned int punch_value = (GenerateRandom() % 4) + 1;
        PlaySoundIfVisible(QString("punch%1.wav").arg(punch_value));

        if (GenerateRandom() % 5 == 0)
        {
            SetLaying(true);
            AddLayingTimer(100);
            PostVisible(name + " has been knocked out!", GetPosition());
        }

        damaged = true;
    }

    if (!damaged)
    {
        return;
    }
}

void Human::Represent(GrowingFrame* frame)
{
    FrameData::Entity ent;
    ent.id = GetId();
    ent.click_id = GetId();
    ent.pos_x = GetPosition().x;
    ent.pos_y = GetPosition().y;
    ent.vlevel = v_level;
    ent.view = *GetView();
    if (!lying_)
    {
        ent.dir = GetDir();
    }
    else
    {
        ent.dir = Dir::SOUTH;
    }
    frame->Append(ent);
}

void Human::CalculateVisible(QVector<Position>* visible_list) const
{
    if (CalculateHealth() >= 0)
    {
        GetGame().GetMap().CalculateLos(
            visible_list,
            GetPosition().x,
            GetPosition().y,
            GetPosition().z);
    }
}

void Human::Bump(IdPtr<Movable> item)
{
    if (IdPtr<Projectile> projectile = item)
    {
        ApplyBruteDamage(projectile->GetDamage() * 100);
        ApplyBurnDamage(projectile->GetBurnDamage() * 100);
        PostVisible(
            name + " got hit by a " + projectile->name + "!", GetPosition());

        // TODO (?): sound
        return;
    }
    Movable::Bump(item);
}

void Human::RotationAction(IdPtr<MapObject> item)
{
    if (IdPtr<Movable> movable = item)
    {
        if (!movable->anchored_)
        {
            if (IdPtr<Projectile> projectile = movable)
            {
                return;
            }
            movable->Rotate(static_cast<Dir>((static_cast<int>(movable->GetDir()) + 1) % 4));
        }
    }
}

void Human::PullAction(IdPtr<MapObject> item)
{
    if (IdPtr<Movable> movable = item)
    {
        if (CanTouch(movable))
        {
            if (IdPtr<Projectile> projectile = movable)
            {
                return;
            }
            pulled_object_ = movable;
            interface_->UpdatePulling(true);
        }
    }
}

void Human::StopPull()
{
    pulled_object_ = 0;
    interface_->UpdatePulling(false);
}

void Human::TryClownBootsHonk()
{
    if (lying_)
    {
        return;
    }
    if (GetGameTick() % 3 != 0)
    {
        return;
    }
    IdPtr<ClownBoots> shoes = GetHumanInterface()->GetItem(slot::FEET);
    if (!shoes.IsValid())
    {
        return;
    }
    if (IdPtr<Space> tile = GetTurf())
    {
        return;
    }
    QString sound = QString("clownstep%1.wav").arg(GenerateRandom() % 2 + 1);
    PlaySoundIfVisible(sound);
}

void Human::MakeEmote(const QString& emote)
{
    GetGame().GetChatFrameInfo().PostVisible(
        QString("<b>%1</b> %2").arg(name.toHtmlEscaped()).arg(emote.toHtmlEscaped()),
        GetPosition());
}

int Human::CalculateHealth() const
{
    return max_health_ - (suffocation_damage_ + burn_damage_ + brute_damage_);
}

void Human::ApplyBurnDamage(int damage)
{
    burn_damage_ += damage;
    burn_damage_ = qMax(0, burn_damage_);
}

void Human::ApplySuffocationDamage(int damage)
{
    suffocation_damage_ += damage;
    suffocation_damage_ = qMax(0, suffocation_damage_);
}

void Human::ApplyBruteDamage(int damage)
{
    brute_damage_ += damage;
    brute_damage_ = qMax(0, brute_damage_);
    if (damage > 0)
    {
        const int ALWAYS_BLOOD_BORDER = 1000;
        int brute_helper = qMax(1, ALWAYS_BLOOD_BORDER - brute_damage_);
        if ((GenerateRandom() % brute_helper) == 1)
        {
            return;
        }
        unsigned int blood_value = (GenerateRandom() % 7) + 1;

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
}
