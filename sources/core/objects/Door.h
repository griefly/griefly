#pragma once

#include "OnMapObject.h"
#include "Movable.h"

namespace kv
{

class Door : public IOnMapObject
{
public:
    DECLARE_SAVEABLE(Door, IOnMapObject);
    REGISTER_CLASS_AS(Door);
    Door();
    
    enum State
    {
        OPEN, CLOSED, OPENING, CLOSING, WELDED  
    };

    virtual void Open();
    virtual void Close();
    virtual void Process() override;
    virtual void Bump(IdPtr<IMovable> item) override;
    virtual void AttackBy(IdPtr<Item> item) override;
    bool IsState(State state) const { return state == door_state_; }
    void Weld();
private:
    int KV_SAVEABLE(door_state_);
    quint32 KV_SAVEABLE(last_tick_);
};
END_DECLARE(Door);

class SecurityDoor : public Door
{
public:
    DECLARE_SAVEABLE(SecurityDoor, Door);
    REGISTER_CLASS_AS(SecurityDoor);
    SecurityDoor();
};
END_DECLARE(SecurityDoor);

class NontransparentDoor : public Door
{
public:
    DECLARE_SAVEABLE(NontransparentDoor, Door);
    REGISTER_CLASS_AS(NontransparentDoor);
    NontransparentDoor();

    virtual void Open() override;
    virtual void Close() override;
};
END_DECLARE(NontransparentDoor);

class ExternalDoor : public NontransparentDoor
{
public:
    DECLARE_SAVEABLE(ExternalDoor, NontransparentDoor);
    REGISTER_CLASS_AS(ExternalDoor);
    ExternalDoor();
};
END_DECLARE(ExternalDoor);

class MaintenanceDoor : public NontransparentDoor
{
public:
    DECLARE_SAVEABLE(MaintenanceDoor, NontransparentDoor);
    REGISTER_CLASS_AS(MaintenanceDoor);
    MaintenanceDoor();
};
END_DECLARE(MaintenanceDoor);

class GlassDoor : public IMovable
{
public:
    DECLARE_SAVEABLE(GlassDoor, IMovable);
    REGISTER_CLASS_AS(GlassDoor);
    GlassDoor();

    virtual void AfterWorldCreation() override;

    enum STATE
    {
        OPEN, CLOSED, OPENING, CLOSING  
    };

    virtual void Open();
    virtual void Close();
    virtual void Process() override;
    virtual void Bump(IdPtr<IMovable> item) override;
    virtual void AttackBy(IdPtr<Item> item) override;
    bool IsState(STATE state) const { return state == door_state_; };
private:
    int KV_SAVEABLE(door_state_);
    quint32 KV_SAVEABLE(last_tick_);

    QString KV_SAVEABLE(door_prefix_);
};
END_DECLARE(GlassDoor);

}
