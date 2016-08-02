#pragma once

#include "OnMapObject.h"
#include "Movable.h"

class Door: public IOnMapObject
{
public:
    DECLARE_SAVED(Door, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(Door);
    Door(size_t id);
    
    virtual void Open();
    virtual void Close();
    virtual void Process() override;
    virtual void Bump(IdPtr<IMovable> item) override;
    virtual void AttackBy(IdPtr<Item> item) override;
    bool IsOpen() const { return door_state_ == OPEN; }
    bool IsClosed() const { return door_state_ == CLOSED; }
    void Weld();
private:
    enum STATE
    {
        OPEN, CLOSED, OPENING, CLOSING, WELDED  
    };
    int KV_SAVEBLE(door_state_);
    size_t KV_SAVEBLE(last_tick_);
};
ADD_TO_TYPELIST(Door);

class SecurityDoor: public Door
{
public:
    DECLARE_SAVED(SecurityDoor, Door);
    DECLARE_GET_TYPE_ITEM(SecurityDoor);
    SecurityDoor(size_t id);
};
ADD_TO_TYPELIST(SecurityDoor);

class NontransparentDoor: public Door
{
public:
    DECLARE_SAVED(NontransparentDoor, Door);
    DECLARE_GET_TYPE_ITEM(NontransparentDoor);
    NontransparentDoor(size_t id);

    virtual void Open() override;
    virtual void Close() override;
};
ADD_TO_TYPELIST(NontransparentDoor);

class ExternalDoor: public NontransparentDoor
{
public:
    DECLARE_SAVED(ExternalDoor, NontransparentDoor);
    DECLARE_GET_TYPE_ITEM(ExternalDoor);
    ExternalDoor(size_t id);
};
ADD_TO_TYPELIST(ExternalDoor);

class MaintenanceDoor: public NontransparentDoor
{
public:
    DECLARE_SAVED(MaintenanceDoor, NontransparentDoor);
    DECLARE_GET_TYPE_ITEM(MaintenanceDoor);
    MaintenanceDoor(size_t id);
};
ADD_TO_TYPELIST(MaintenanceDoor);

class GlassDoor: public IMovable
{
public:
    DECLARE_SAVED(GlassDoor, IMovable);
    DECLARE_GET_TYPE_ITEM(GlassDoor);
    GlassDoor(size_t id);

    virtual void AfterWorldCreation() override;

    virtual void Open();
    virtual void Close();
    virtual void Process() override;
    virtual void Bump(IdPtr<IMovable> item) override;
    virtual void AttackBy(IdPtr<Item> item) override;
    bool IsOpen() const { return door_state_ == OPEN; }
private:
    enum STATE
    {
        OPEN, CLOSED, OPENING, CLOSING
    };
    int KV_SAVEBLE(door_state_);
    size_t KV_SAVEBLE(last_tick_);

    std::string KV_SAVEBLE(door_prefix_);
};
ADD_TO_TYPELIST(GlassDoor);
