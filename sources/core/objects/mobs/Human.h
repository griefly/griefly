#pragma once

#include "Mob.h"
#include "Hearer.h"

#include "HumanInterface.h"

const int HUMAN_MAX_HEALTH = 10000;
const int REGULAR_TEMPERATURE = 40;

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
  
    virtual void Bump(IdPtr<Movable> item) override;

    virtual void AttackBy(IdPtr<Item> item) override;

    virtual void Represent(GrowingFrame* frame) override;

    virtual bool TryMove(Dir direct) override;

    IdPtr<HumanInterface> GetHumanInterface() { return interface_; }

    virtual void CalculateVisible(VisiblePoints* visible_list) const override;

    void UpdateOverlays();

    int GetHealth() { return CalculateHealth(); }

    void RotationAction(IdPtr<MapObject> item);
    void PullAction(IdPtr<MapObject> item);
    void StopPull();
    
    int GetBurnDamage() { return burn_damage_; }
    int GetSuffocationDamage() { return suffocation_damage_; }
    int GetBruteDamage() { return brute_damage_; }
    bool IsDead() { return dead_; }
    void ApplyBurnDamage(int damage);
    void ApplySuffocationDamage(int damage);
    void ApplyBruteDamage(int damage);
protected:
    int KV_SAVEABLE(attack_cooldown_);

    IdPtr<HumanInterface> KV_SAVEABLE(interface_);

    int KV_SAVEABLE(lay_timer_);

    bool KV_SAVEABLE(lying_);
    bool KV_SAVEABLE(dead_);

    int KV_SAVEABLE(max_health_);
    int KV_SAVEABLE(suffocation_damage_);
    int KV_SAVEABLE(burn_damage_);
    int KV_SAVEABLE(brute_damage_);

    IdPtr<Movable> KV_SAVEABLE(pulled_object_);
private:
    void TryClownBootsHonk();
    void MakeEmote(const QString& emote);
};
END_DECLARE(Human);

}
