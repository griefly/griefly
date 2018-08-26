#pragma once

#include "Mob.h"
#include "Hearer.h"

#include "HumanInterface.h"
#include "objects/movable/items/Limbs.h"

const int HUMAN_MAX_HEALTH = 10000;
const int REGULAR_TEMPERATURE = 40;
const int HUMAN_PLASMA_TOXINS_THRESHOLD = 35;
const int HUMAN_PLASMA_TOXINS_DAMAGE = 50;

namespace kv
{

class Human : public Mob, public Hearer
{
public:
    DECLARE_SAVEABLE(Human, Mob);
    REGISTER_CLASS_AS(Human);

    virtual Hearer* ToHearer() override { return this; }
    virtual QVector<Position> GetHeardPoints() const override;
    virtual void Hear(const Phrase& phrase) override;

    Human();

    virtual void AfterWorldCreation() override;

    virtual void MindExit() override;
    virtual void MindEnter() override;
    virtual void GenerateInterfaceForFrame(GrowingFrame* representation) override;
    virtual void ProcessMessage(const Message& message) override;
    virtual void Process() override;
    virtual void Live();
    virtual void Regeneration();

    virtual IdPtr<MapObject> GetNeighbour(Dir) const;

    virtual void OnDeath();
    int CalculateHealth() const;

    void SetLaying(bool value);
    bool GetLying() const { return lying_; }

    void AddLayingTimer(int value);
  
    virtual void ApplyForce(const Vector& force, ForceSource source = ForceSource::UNKNOWN) override;

    virtual void Bump(const Vector& force, IdPtr<Movable> item) override;

    virtual bool RemoveObject(IdPtr<MapObject> object) override;

    virtual void AttackBy(IdPtr<Item> item) override;

    virtual void Represent(GrowingFrame* frame, IdPtr<Mob> mob) const override;

    virtual bool TryMove(Dir direct) override;
    virtual bool Rotate(Dir direct) override;

    IdPtr<HumanInterface> GetHumanInterface() { return interface_; }

    virtual void CalculateVisible(VisiblePoints* visible_list) const override;

    void UpdateOverlays();

    int GetHealth() const { return CalculateHealth(); }

    void RotationAction(IdPtr<MapObject> item);
    void PullAction(IdPtr<MapObject> item);
    void StopPull();
    
    int GetBurnDamage() const { return burn_damage_; }
    int GetSuffocationDamage() const { return suffocation_damage_; }
    int GetBruteDamage() const { return brute_damage_; }
    int GetToxinsDamage() const { return toxins_damage_; }
    bool IsDead() const { return dead_; }
    void ApplyBurnDamage(int damage);
    void ApplySuffocationDamage(int damage);
    void ApplyBruteDamage(int damage);
    void ApplyToxinsDamage(int damage);
protected:
    int KV_SAVEABLE(attack_cooldown_);

    IdPtr<HumanInterface> KV_SAVEABLE(interface_);

    int KV_SAVEABLE(lay_timer_);

    bool KV_SAVEABLE(lying_);
    bool KV_SAVEABLE(dead_);

    int KV_SAVEABLE(max_health_);
    int KV_SAVEABLE(suffocation_damage_);
    int KV_SAVEABLE(toxins_damage_);
    int KV_SAVEABLE(burn_damage_);
    int KV_SAVEABLE(brute_damage_);

    IdPtr<Movable> KV_SAVEABLE(pulled_object_);
private:
    void TryClownBootsHonk();
    void MakeEmote(const QString& emote);

    // TODO: limbs?
    IdPtr<Hand> KV_SAVEABLE(hand_);
};
END_DECLARE(Human);

}
