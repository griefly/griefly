#pragma once

#include "Mob.h"

const int HUMAN_MAX_HEALTH = 10000;
const int REGULAR_TEMPERATURE = 40;

class Human : public IMob
{
public:
    DECLARE_SAVED(Human, IMob);
    DECLARE_GET_TYPE_ITEM(Human);
    Human();

    virtual void AfterWorldCreation() override;

    virtual void DeinitGUI() override;
    virtual void InitGUI() override;
    virtual void GenerateInterfaceForFrame() override;
    virtual void ProcessMessage(const Message2& msg) override;
    virtual void Process() override;
    virtual void Live();
    virtual void Regeneration();

    virtual IdPtr<IOnMapBase> GetNeighbour(Dir) const;

    virtual void OnDeath();
    int CalculateHealth();

    void SetLying(bool value);
    bool GetLying() const { return lying_; }

    void AddLyingTimer(int value);
  
    virtual void Bump(IdPtr<IMovable> item) override;

    virtual void AttackBy(IdPtr<kv::Item> item) override;

    virtual void Represent() override;

    virtual bool TryMove(Dir direct) override;
    virtual InterfaceBase* GetInterface() override { return GetHumanInterface(); }
    
    HumanInterface* GetHumanInterface() { return &interface_; }

    virtual void CalculateVisible(std::list<PosPoint>* visible_list) override;

    void UpdateOverlays();

    int GetHealth() { return CalculateHealth(); }

    void RotationAction(IdPtr<IOnMapBase> item);
    void PullAction(IdPtr<IOnMapBase> item);
    void StopPull();
    
    int GetBurnDamage() { return burn_damage_; }
    int GetSuffocationDamage() { return suffocation_damage_; }
    int GetBruteDamage() { return brute_damage_; }
    bool IsDead() { return dead_; }
    void ApplyBurnDamage(int damage);
    void ApplySuffocationDamage(int damage);
    void ApplyBruteDamage(int damage);
protected:
    int KV_SAVEBLE(attack_cooldown_);

    HumanInterface KV_SAVEBLE(interface_);

    int KV_SAVEBLE(lay_timer_);

    bool KV_SAVEBLE(lying_);
    bool KV_SAVEBLE(dead_);

    int KV_SAVEBLE(max_health_);
    int KV_SAVEBLE(suffocation_damage_);
    int KV_SAVEBLE(burn_damage_);
    int KV_SAVEBLE(brute_damage_);

    IdPtr<IMovable> KV_SAVEBLE(pulled_object_);
private:
    void TryClownBootsHonk();
    void MakeEmote(const QString& emote);
};
ADD_TO_TYPELIST(Human);
