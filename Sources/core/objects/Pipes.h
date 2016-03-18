#pragma once

#include "Movable.h"
#include "../AtmosHolder.h"

class PipeBase: public IMovable
{
public:
    DECLARE_SAVED(PipeBase, IMovable);
    DECLARE_GET_TYPE_ITEM(PipeBase);
    PipeBase(size_t id);
    virtual bool Connect(Dir dir, id_ptr_on<PipeBase> pipe) { return false; }
protected:
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
private:
    static void GetTailAndHead(Dir dir, Dir* head, Dir* tail);

    id_ptr_on<PipeBase> KV_SAVEBLE(head_);
    id_ptr_on<PipeBase> KV_SAVEBLE(tail_);
};
ADD_TO_TYPELIST(Pipe);


