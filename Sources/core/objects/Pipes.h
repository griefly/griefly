#pragma once

#include "Movable.h"
#include "Item.h"

#include "../AtmosHolder.h"

class PipeBase: public IMovable
{
public:
    DECLARE_SAVED(PipeBase, IMovable);
    DECLARE_GET_TYPE_ITEM(PipeBase);
    PipeBase(size_t id);
    virtual bool Connect(Dir dir, id_ptr_on<PipeBase> pipe) { return false; }
    virtual bool CanTransferGas(Dir dir) const { return false; }
    AtmosHolder* GetAtmosHolder() { return &atmos_holder_; }
protected:
    void ConnectHelper(id_ptr_on<PipeBase>& connection, Dir dir);
    void ProcessHelper(id_ptr_on<PipeBase>& connection, Dir dir);
    AtmosHolder KV_SAVEBLE(atmos_holder_);
};
ADD_TO_TYPELIST(PipeBase);

class Pipe: public PipeBase
{
public:
    DECLARE_SAVED(Pipe, PipeBase);
    DECLARE_GET_TYPE_ITEM(Pipe);
    Pipe(size_t id);
    virtual bool Connect(Dir dir, id_ptr_on<PipeBase> pipe) override;
    virtual void AfterWorldCreation() override;
    virtual bool CanTransferGas(Dir dir) const override { return true; }
    virtual void Process() override;
private:
    static void GetTailAndHead(Dir dir, Dir* head, Dir* tail);
    id_ptr_on<PipeBase> KV_SAVEBLE(head_);
    id_ptr_on<PipeBase> KV_SAVEBLE(tail_);
};
ADD_TO_TYPELIST(Pipe);

class Manifold: public PipeBase
{
public:
    DECLARE_SAVED(Manifold, PipeBase);
    DECLARE_GET_TYPE_ITEM(Manifold);
    Manifold(size_t id);
    virtual bool Connect(Dir dir, id_ptr_on<PipeBase> pipe) override;
    virtual void AfterWorldCreation() override;
    virtual bool CanTransferGas(Dir dir) const override;
    virtual void Process() override;
private:
    static void GetConnectionsDirs(Dir dir, Dir* tail, Dir* left, Dir* right);

    id_ptr_on<PipeBase> KV_SAVEBLE(tail_);
    id_ptr_on<PipeBase> KV_SAVEBLE(left_);
    id_ptr_on<PipeBase> KV_SAVEBLE(right_);
};
ADD_TO_TYPELIST(Manifold);


class Valve: public Pipe
{
public:
    DECLARE_SAVED(Valve, Pipe);
    DECLARE_GET_TYPE_ITEM(Valve);
    Valve(size_t id);
    virtual bool CanTransferGas(Dir dir) const override;
    virtual void Process() override;
    virtual void AttackBy(id_ptr_on<Item> item) override;
private:
    bool KV_SAVEBLE(closed_);
};
ADD_TO_TYPELIST(Valve);

class Vent: public PipeBase
{
public:
    DECLARE_SAVED(Vent, PipeBase);
    DECLARE_GET_TYPE_ITEM(Vent);
    Vent(size_t id);
    virtual bool Connect(Dir dir, id_ptr_on<PipeBase> pipe) override;
    virtual void AfterWorldCreation() override;
    virtual bool CanTransferGas(Dir dir) const override { return true; }
    virtual void Process() override;
private:
    id_ptr_on<PipeBase> KV_SAVEBLE(tail_);
};
ADD_TO_TYPELIST(Vent);

class Connector: public PipeBase
{
public:
    DECLARE_SAVED(Connector, PipeBase);
    DECLARE_GET_TYPE_ITEM(Connector);
    Connector(size_t id);
    virtual bool Connect(Dir dir, id_ptr_on<PipeBase> pipe) override;
    virtual void AfterWorldCreation() override;
    virtual bool CanTransferGas(Dir dir) const override { return true; }
    virtual void Process() override;
private:
    id_ptr_on<PipeBase> KV_SAVEBLE(tail_);
};
ADD_TO_TYPELIST(Connector);


