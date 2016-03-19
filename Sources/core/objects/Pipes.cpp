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
