#pragma once

#include "Movable.h"

#include "../atmos/AtmosHolder.h"

#include "Item.h"
#include "GasTank.h"

namespace kv
{

class PipeBase : public Movable
{
public:
    DECLARE_SAVEABLE(PipeBase, Movable);
    REGISTER_CLASS_AS(PipeBase);
    PipeBase();
    virtual void AttackBy(IdPtr<Item> item) override;
    virtual bool Connect(Dir dir, IdPtr<PipeBase> pipe) { return false; }
    virtual bool CanTransferGas(Dir dir) const { return false; }
    atmos::AtmosHolder* GetAtmosHolder() { return &atmos_holder_; }
protected:
    void ConnectHelper(IdPtr<PipeBase>& connection, Dir dir);
    void ProcessHelper(IdPtr<PipeBase>& connection, Dir dir);
    atmos::AtmosHolder KV_SAVEABLE(atmos_holder_);
};
END_DECLARE(PipeBase);

class Pipe : public PipeBase
{
public:
    DECLARE_SAVEABLE(Pipe, PipeBase);
    REGISTER_CLASS_AS(Pipe);
    Pipe();
    virtual bool Connect(Dir dir, IdPtr<PipeBase> pipe) override;
    virtual void AfterWorldCreation() override;
    virtual bool CanTransferGas(Dir dir) const override { return true; }
    virtual void Process() override;
protected:
    static void GetTailAndHead(Dir dir, Dir* head, Dir* tail);
    IdPtr<PipeBase> KV_SAVEABLE(head_);
    IdPtr<PipeBase> KV_SAVEABLE(tail_);
};
END_DECLARE(Pipe);

class Manifold : public PipeBase
{
public:
    DECLARE_SAVEABLE(Manifold, PipeBase);
    REGISTER_CLASS_AS(Manifold);
    Manifold();
    virtual bool Connect(Dir dir, IdPtr<PipeBase> pipe) override;
    virtual void AfterWorldCreation() override;
    virtual bool CanTransferGas(Dir dir) const override;
    virtual void Process() override;
private:
    static void GetConnectionsDirs(Dir dir, Dir* tail, Dir* left, Dir* right);

    IdPtr<PipeBase> KV_SAVEABLE(tail_);
    IdPtr<PipeBase> KV_SAVEABLE(left_);
    IdPtr<PipeBase> KV_SAVEABLE(right_);
};
END_DECLARE(Manifold);


class Valve : public Pipe
{
public:
    DECLARE_SAVEABLE(Valve, Pipe);
    REGISTER_CLASS_AS(Valve);
    Valve();
    virtual bool CanTransferGas(Dir dir) const override;
    virtual void Process() override;
    virtual void AttackBy(IdPtr<Item> item) override;
private:
    bool KV_SAVEABLE(closed_);
};
END_DECLARE(Valve);

class Vent : public PipeBase
{
public:
    DECLARE_SAVEABLE(Vent, PipeBase);
    REGISTER_CLASS_AS(Vent);
    Vent();
    virtual bool Connect(Dir dir, IdPtr<PipeBase> pipe) override;
    virtual void AfterWorldCreation() override;
    virtual bool CanTransferGas(Dir dir) const override { return true; }
    virtual void Process() override;

    void SetHidden(bool hidden);
private:
    IdPtr<PipeBase> KV_SAVEABLE(tail_);
};
END_DECLARE(Vent);

class Connector : public PipeBase
{
public:
    DECLARE_SAVEABLE(Connector, PipeBase);
    REGISTER_CLASS_AS(Connector);
    Connector();
    void ConnectToGasTank(IdPtr<GasTank> tank);
    void DisconnectFromGasTank();
    virtual bool Connect(Dir dir, IdPtr<PipeBase> pipe) override;
    virtual void AfterWorldCreation() override;
    virtual bool CanTransferGas(Dir dir) const override { return true; }
    virtual void Process() override;
private:
    IdPtr<GasTank> KV_SAVEABLE(tank_);
    IdPtr<PipeBase> KV_SAVEABLE(tail_);
};
END_DECLARE(Connector);

class PipePump : public Pipe
{
public:
    DECLARE_SAVEABLE(PipePump, Pipe);
    REGISTER_CLASS_AS(PipePump);
    PipePump();
    virtual bool CanTransferGas(Dir dir) const override;
    virtual void Process() override;
public:
    int KV_SAVEABLE(pump_pressure_);
};
END_DECLARE(PipePump);

}
