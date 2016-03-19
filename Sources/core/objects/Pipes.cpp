#include "Pipes.h"

#include "../Helpers.h"

PipeBase::PipeBase(size_t id) : IMovable(id)
{
    anchored = true;
    v_level = 3;

    SetSprite("icons/pipes.dmi");
    //SetState("intact");

    name = "Please do not create me";
}


Pipe::Pipe(size_t id) : PipeBase(id)
{
    SetState("intact");

    name = "Pipe";

    SetFreq(1);
}

bool Pipe::Connect(Dir dir, id_ptr_on<PipeBase> pipe)
{
    Dir head;
    Dir tail;
    GetTailAndHead(GetDir(), &head, &tail);

    if (   (dir == head)
        && (!head_.valid()))
    {
        head_ = pipe;
        return true;
    }
    if (   (dir == tail)
        && (!tail_.valid()))
    {
        tail_ = pipe;
        return true;
    }

    return false;
}

void Pipe::AfterWorldCreation()
{
    PipeBase::AfterWorldCreation();

    Dir head;
    Dir tail;
    GetTailAndHead(GetDir(), &head, &tail);

    if (!head_.valid())
    {
        GetNeighbour(head)->ForEach(
        [&](id_ptr_on<IOnMapBase> obj)
        {
            if (head_.valid())
            {
                return;
            }
            if (id_ptr_on<PipeBase> pipe = obj)
            {
                if (pipe->Connect(helpers::revert_dir(head), GetId()))
                {
                    head_ = pipe;
                }
            }
        });
    }
    if (!tail_.valid())
    {
        GetNeighbour(tail)->ForEach(
        [&](id_ptr_on<IOnMapBase> obj)
        {
            if (tail_.valid())
            {
                return;
            }
            if (id_ptr_on<PipeBase> pipe = obj)
            {
                if (pipe->Connect(helpers::revert_dir(tail), GetId()))
                {
                    tail_ = pipe;
                }
            }
        });
    }
}

void Pipe::Process()
{
    Dir head;
    Dir tail;
    GetTailAndHead(GetDir(), &head, &tail);

    if (head_.valid())
    {
        if (head_->CanTransferGas(helpers::revert_dir(head)))
        {
            head_->GetAtmosHolder()->Connect(GetAtmosHolder());
        }
    }
    else
    {
        if (id_ptr_on<CubeTile> cube = owner)
        {
            cube->GetAtmosHolder()->Connect(GetAtmosHolder());
        }
    }
    if (tail_.valid())
    {
        if (tail_->CanTransferGas(helpers::revert_dir(tail)))
        {
            tail_->GetAtmosHolder()->Connect(GetAtmosHolder());
        }
    }
    else
    {
        if (id_ptr_on<CubeTile> cube = owner)
        {
            cube->GetAtmosHolder()->Connect(GetAtmosHolder());
        }
    }
}

void Pipe::GetTailAndHead(Dir dir, Dir* head, Dir* tail)
{
    static int DIRS_DATA[10][2]
        = {
        {D_LEFT, D_RIGHT}, // D_LEFT
        {D_RIGHT, D_LEFT}, // D_RIGHT
        {D_UP, D_DOWN}, // D_UP
        {D_DOWN, D_UP}, // D_DOWN
        {D_ZUP, D_ZDOWN}, // D_ZUP
        {D_ZDOWN, D_ZUP}, // D_ZDOWN
        {D_RIGHT, D_DOWN}, // D_SOUTHEAST
        {D_DOWN, D_LEFT}, // D_SOUTHWEST
        {D_UP, D_RIGHT}, // D_NORTHEAST
        {D_LEFT, D_UP} // D_NORTHWEST
        };
    *head = DIRS_DATA[dir][0];
    *tail = DIRS_DATA[dir][1];
}

Manifold::Manifold(size_t id) : PipeBase(id)
{
    SetState("manifold");
    name = "Manifold";
    SetFreq(1);
}

bool Manifold::Connect(Dir dir, id_ptr_on<PipeBase> pipe)
{
    // TODO
    return false;
}

void Manifold::AfterWorldCreation()
{
    // TODO
    PipeBase::AfterWorldCreation();
}

bool Manifold::CanTransferGas(Dir dir) const
{
    // TODO
    return true;
}

void Manifold::Process()
{
    // TODO
}

void Manifold::GetConnectionsDirs(Dir dir, Dir *tail, Dir *left, Dir *right)
{
    // TODO
}

Vent::Vent(size_t id) : PipeBase(id)
{
    SetState("vent_filter-4");

    name = "Vent";

    SetFreq(1);
}

bool Vent::Connect(Dir dir, id_ptr_on<PipeBase> pipe)
{
    if (dir != GetDir())
    {
        return false;
    }
    if (tail_.valid())
    {
        return false;
    }

    tail_ = pipe;
    return true;
}

void Vent::AfterWorldCreation()
{
    PipeBase::AfterWorldCreation();

    if (!tail_.valid())
    {
        GetNeighbour(GetDir())->ForEach(
        [&](id_ptr_on<IOnMapBase> obj)
        {
            if (tail_.valid())
            {
                return;
            }
            if (id_ptr_on<PipeBase> pipe = obj)
            {
                if (pipe->Connect(helpers::revert_dir(GetDir()), GetId()))
                {
                    tail_ = pipe;
                }
            }
        });
    }
}

void Vent::Process()
{
    if (tail_.valid())
    {
        if (tail_->CanTransferGas(helpers::revert_dir(GetDir())))
        {
            tail_->GetAtmosHolder()->Connect(GetAtmosHolder());
        }
    }
    if (id_ptr_on<CubeTile> cube = owner)
    {
        cube->GetAtmosHolder()->Connect(GetAtmosHolder());
    }
}

Valve::Valve(size_t id) : Pipe(id)
{
    SetSprite("icons/digital_valve.dmi");
    SetState("valve0");
    closed_ = true;
}

bool Valve::CanTransferGas(Dir dir) const
{
    if (closed_)
    {
        return false;
    }
    return true;
}

void Valve::Process()
{
    if (closed_)
    {
        return;\
    }
    Pipe::Process();
}

void Valve::AttackBy(id_ptr_on<Item> item)
{
    if (closed_)
    {
        closed_ = false;
        SetState("valve1");
    }
    else
    {
        closed_ = true;
        SetState("valve0");
    }
}
