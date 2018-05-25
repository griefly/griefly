#pragma once

#include "MaterialObject.h"
#include "movable/Movable.h"

namespace kv
{

class Door : public MaterialObject
{
public:
    DECLARE_SAVEABLE(Door, MaterialObject);
    REGISTER_CLASS_AS(Door);
    Door();
    
    enum class State : qint32
    {
        OPEN,
        CLOSED,
        OPENING,
        CLOSING,
        WELDED
    };

    virtual void Open();
    virtual void Close();
    virtual void Process() override;
    virtual void Bump(const Vector& vector, IdPtr<Movable> item) override;
    virtual void AttackBy(IdPtr<Item> item) override;
    bool IsState(State state) const { return state == door_state_; }
    void Weld();
private:
    State KV_SAVEABLE(door_state_);
    qint32 KV_SAVEABLE(last_tick_);
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

class GlassDoor : public Movable
{
public:
    DECLARE_SAVEABLE(GlassDoor, Movable);
    REGISTER_CLASS_AS(GlassDoor);
    GlassDoor();

    virtual void AfterWorldCreation() override;

    enum class State : qint32
    {
        OPEN,
        CLOSED,
        OPENING,
        CLOSING
    };

    virtual void Open();
    virtual void Close();
    virtual void Process() override;
    virtual void Bump(const Vector& vector, IdPtr<Movable> item) override;
    virtual void AttackBy(IdPtr<Item> item) override;
    bool IsState(State state) const { return state == door_state_; }
private:
    State KV_SAVEABLE(door_state_);
    qint32 KV_SAVEABLE(last_tick_);

    QString KV_SAVEABLE(door_prefix_);
};
END_DECLARE(GlassDoor);

}
