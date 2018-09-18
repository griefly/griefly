#include "Human.h"

#include "ObjectFactory.h"
#include "Names.h"
#include "objects/movable/items/Shard.h"

#include "core_headers/NetworkMessages.h"

#include "atmos/AtmosHolder.h"
#include "objects/Tile.h"
#include "SynchronizedRandom.h"
#include "Ghost.h"
#include "objects/movable/items/Clothes.h"
#include "objects/turfs/Floor.h"
#include "objects/Lobby.h"
#include "objects/movable/items/Gun.h"
#include "objects/movable/items/Weapons.h"
#include "objects/movable/items/ElectricTools.h"
#include "objects/turfs/Space.h"
#include "objects/movable/items/Drinks.h"
#include "objects/movable/items/MedbayTools.h"
#include "objects/PhysicsEngine.h"
#include "objects/movable/items/HandGasTank.h"
#include "objects/movable/items/Masks.h"

#include "ChatFrameInfo.h"

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
        QString string_to_chat = QString("<b>%1</b> <i>%2</i>").arg(phrase.from).arg(phrase.expression);
        if (!phrase.text.isEmpty())
        {
            string_to_chat.append(QString(", <span>\"%1\"</span>").arg(phrase.text));
        }
        GetGame().GetChatFrameInfo().PostPersonal(string_to_chat, GetGame().GetNetId(GetId()));
    }
}

Human::Human()
{
    SetTickSpeed(1);
    SetSprite("icons/human.dmi");
    SetState("african1_m_s");
    SetPassable(Dir::ALL, passable::BIG_ITEM);
    SetVisibleLevel(visible_level::STANDING_MOB);
    attack_cooldown_ = 0;
    SetName("Morgan James");
    SetPassableLevel(passable::BIG_CREATURE);

    lay_timer_ = 0;

    dead_ = false;
    lying_ = false;
    max_health_ = HUMAN_MAX_HEALTH;
    suffocation_damage_ = 0;
    burn_damage_ = 0;
    brute_damage_ = 0;
    toxins_damage_ = 0;

    pulled_object_ = 0;
}

void Human::AfterWorldCreation()
{
    Mob::AfterWorldCreation();

    interface_ = Create<HumanInterface>();
    interface_->SetOwner(GetId());

    // TODO: hand can be deleted, so #290
    hand_ = Create<Hand>();
    hand_->SetOwner(GetId());

    UpdateOverlays();

    SetName(GetGame().GetNames().GetMaleName());
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

bool Human::Rotate(Dir direct)
{
    if (Mob::Rotate(direct))
    {
        UpdateOverlays();
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
        const Vector& force = GetForce();
        if (std::abs(force.x) + std::abs(force.y) + std::abs(force.z) < (4 * FORCE_UNIT))
        {
            if (IsKey(message.data, Input::MOVE_UP))
            {
                ApplyForce(DirToVDir(Dir::NORTH) * FORCE_UNIT);
                return;
            }
            else if (IsKey(message.data, Input::MOVE_DOWN))
            {
                ApplyForce(DirToVDir(Dir::SOUTH) * FORCE_UNIT);
                return;
            }
            else if (IsKey(message.data, Input::MOVE_LEFT))
            {
                ApplyForce(DirToVDir(Dir::WEST) * FORCE_UNIT);
                return;
            }
            else if (IsKey(message.data, Input::MOVE_RIGHT))
            {
                ApplyForce(DirToVDir(Dir::EAST) * FORCE_UNIT);
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
                qint32 length = str.length();
                text.replace(0, length, "");
                MakeEmote(text);
                found = true;
                break;
            }
        }
        if (!found)
        {
            Phrase phrase;
            phrase.from = GetName().toHtmlEscaped();
            phrase.text = text.toHtmlEscaped().trimmed();

            phrase.expression = [&]()
            {
                if (phrase.text.isEmpty())
                {
                    return "keeps silence";
                }

                auto lastchar_position = phrase.text.length() - 1;
                auto lastchar = phrase.text[lastchar_position];
                switch (lastchar.unicode())
                {
                case '!':
                    return "exclaims";
                case '?':
                    return "asks";
                default:
                    return "says";
                }
            }();

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

        IdPtr<MaterialObject> object = ExtractObjId(message.data);
        if (!object.IsValid())
        {
            return;
        }

        QString action = ExtractAction(message.data);
        if (action == Click::LEFT_SHIFT)
        {
            if (IdPtr<Human> human = object)
            {
                PostVisible(
                    GetName() + " looks at " + human->GetName(),
                    GetPosition());
                return;
            }
            PostVisible(
                GetName() + " looks at the " + object->GetName(),
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
                            kv::Abort("Unable to RemoveItem from GetOwner() during the pick phase!");
                        }
                        object->SetOwner(GetId());
                    }
                    else
                    {
                        interface_->DropItem();
                        object->AttackBy(hand_);
                    }
                }
                else
                {
                    if (GetLying() == false)
                    {
                        PerformAttack(object, interface_->GetItemInActiveHand());
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
    GetView().RemoveOverlays();
    GetView().RemoveUnderlays();
    interface_->AddOverlays(GetDir(), &GetView());
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
        PostVisible(GetName() + " is lying now", GetPosition());
        GetView().SetAngle(90);
        SetPassable(Dir::ALL, passable::FULL);
        SetVisibleLevel(visible_level::LAYING_MOB);
    }
    else
    {
        PostVisible(GetName() + " is standing now!", GetPosition());
        GetView().SetAngle(0);
        SetPassable(Dir::ALL, passable::BIG_ITEM);
        SetVisibleLevel(visible_level::STANDING_MOB);
    }
    interface_->UpdateLaying(lying_);
}

void Human::AddLayingTimer(int value)
{
    lay_timer_ += value;
}

void Human::ApplyForce(const Vector& force, ForceSource source)
{
    if (source == ForceSource::GAS)
    {
        if (IsMinded() && ((GetGameTick() % 5) == 0))
        {
            GetGame().GetChatFrameInfo().PostPersonal(
                "You've been pused by a rush of air!",
                GetGame().GetNetId(GetId()));
        }
    }
    Mob::ApplyForce(force, source);
}

void Human::Live()
{
    if (dead_)
    {
        return;
    }

    // TODO: consider to have always valid owners
    if (!GetOwner().IsValid())
    {
        return;
    }

    if (atmos::AtmosHolder* holder = GetOwner()->GetAtmosHolder())
    {
        const int oxygen = holder->GetGase(atmos::OXYGEN);
        const int plasma = holder->GetGase(atmos::PLASMA);
        const int temperature = holder->GetTemperature();
        const int pressure = holder->GetPressure();

        interface_->UpdateEnvironment(temperature, pressure, oxygen, plasma);

        const int BURNING_THRESHOLD = 3;
        const int MIN_BURN_DAMAGE = 1;
        if (std::abs(REGULAR_TEMPERATURE - temperature) > BURNING_THRESHOLD)
        {
            const int damage = std::max(MIN_BURN_DAMAGE, std::abs(REGULAR_TEMPERATURE - temperature));
            ApplyBurnDamage(damage);
            if ((GetGameTick() % 13) == 0)
            {
                if (   (temperature > REGULAR_TEMPERATURE)
                    && (pressure > ZERO_TEMPERATURE_PRESSURE_BORDER))
                {
                    PostHtmlFor("<font color=\"#FF1200\">It is too hot here!</font>", GetId());
                }
                else
                {
                    PostHtmlFor("<font color=\"#0022FF\">It is too cold here!</font>", GetId());
                }
            }
        }
        if (plasma > HUMAN_PLASMA_TOXINS_THRESHOLD)
        {
            if (CalculateHealth() >= 0)
            {
                holder->RemoveGase(atmos::PLASMA, 1);
                ApplyToxinsDamage(HUMAN_PLASMA_TOXINS_DAMAGE);
            }
        }
        if (oxygen > 0)
        {
            holder->RemoveGase(atmos::OXYGEN, 1);
            holder->AddGase(atmos::CO2, 1);
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
        auto ghost = Create<Ghost>(GetOwner());
        ghost->SetName(GetName());

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

    if (IdPtr<Hand> hand = item)
    {
        ApplyBruteDamage(hand->GetDamage() * 100);

        unsigned int punch_value = (GenerateRandom() % 4) + 1;
        PlaySoundIfVisible(QString("punch%1.wav").arg(punch_value));

        if (GenerateRandom() % 5 == 0)
        {
            SetLaying(true);
            AddLayingTimer(100);
            PostVisible(GetName() + " has been knocked out!", GetPosition());
        }
        return;
    }

    if (item.IsValid() && (item->GetDamage() > 0))
    {
        ApplyBruteDamage(item->GetDamage() * 100);
        QString sound = QString("genhit%1.wav").arg(GenerateRandom() % 3 + 1);
        PlaySoundIfVisible(sound);
        if (IdPtr<MaterialObject> item_owner = item->GetOwner())
        {
            PostVisible(
                QString("<font color=\"red\">%1 is attacked by %2 with %3</font>")
                    .arg(GetName()).arg(item_owner->GetName()).arg(item->GetName()),
                GetPosition());
        }
    }
}

void Human::Represent(GrowingFrame* frame, IdPtr<kv::Mob> mob) const
{
    FrameData::Entity ent;
    ent.id = GetId();
    ent.click_id = GetId();
    ent.pos_x = GetPosition().x;
    ent.pos_y = GetPosition().y;
    ent.vlevel = GetVisibleLevel();
    ent.view = GetView().GetRawData();
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

void Human::Bump(const Vector& force, IdPtr<Movable> item)
{
    if (IdPtr<Projectile> projectile = item)
    {
        ApplyBruteDamage(projectile->GetDamage() * 100);
        ApplyBurnDamage(projectile->GetBurnDamage() * 100);
        PostVisible(
            GetName() + " got hit by a " + projectile->GetName() + "!", GetPosition());

        // TODO (?): sound
        return;
    }
    Movable::Bump(force, item);
}

bool Human::RemoveObject(IdPtr<MapObject> object)
{
    if (interface_->RemoveItem(object))
    {
        UpdateOverlays();
        return true;
    }
    return false;
}

void Human::RotationAction(IdPtr<MapObject> item)
{
    if (IdPtr<Movable> movable = item)
    {
        if (!movable->IsAnchored())
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

void Human::InterfaceChanges(IdPtr<Item> item)
{
    // TODO
    Q_UNUSED(item)
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
        QString("<b>%1</b> %2").arg(GetName().toHtmlEscaped()).arg(emote.toHtmlEscaped()),
        GetPosition());
}

int Human::CalculateHealth() const
{
    return max_health_ - (suffocation_damage_ + burn_damage_ + brute_damage_ + toxins_damage_);
}

void Human::ApplyBurnDamage(int damage)
{
    burn_damage_ += damage;
    burn_damage_ = std::max(0, burn_damage_);
}

void Human::ApplySuffocationDamage(int damage)
{
    suffocation_damage_ += damage;
    suffocation_damage_ = std::max(0, suffocation_damage_);
}

void Human::ApplyToxinsDamage(int damage)
{
    toxins_damage_ += damage;
    toxins_damage_ = std::max(0, toxins_damage_);
}

void Human::ApplyBruteDamage(int damage)
{
    brute_damage_ += damage;
    brute_damage_ = std::max(0, brute_damage_);
    if (damage > 0)
    {
        const int ALWAYS_BLOOD_BORDER = 1000;
        int brute_helper = std::max(1, ALWAYS_BLOOD_BORDER - brute_damage_);
        if ((GenerateRandom() % brute_helper) == 1)
        {
            return;
        }
        unsigned int blood_value = (GenerateRandom() % 7) + 1;

        if (IdPtr<Floor> floor = GetTurf())
        {
            if (!floor->IsBloody())
            {
                floor->GetView().AddOverlay(
                    "icons/blood.dmi",
                    QString("floor%1").arg(blood_value));
                floor->SetBloody(true);
            }
        }
    }
}
