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

void PipeBase::ConnectHelper(id_ptr_on<PipeBase>& connection, Dir dir)
{
    if (!connection.valid())
    {
        GetNeighbour(dir)->ForEach(
        [&](id_ptr_on<IOnMapBase> obj)
        {
            if (connection.valid())
            {
                return;
            }
            if (id_ptr_on<PipeBase> pipe = obj)
            {
                if (pipe->Connect(helpers::revert_dir(dir), GetId()))
                {
                    connection = pipe;
                }
            }
        });
    }
}

void PipeBase::ProcessHelper(id_ptr_on<PipeBase>& connection, Dir dir)
{
    if (connection.valid())
    {
        if (connection->CanTransferGas(helpers::revert_dir(dir)))
        {
            connection->GetAtmosHolder()->Connect(GetAtmosHolder());
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

    ConnectHelper(head_, head);
    ConnectHelper(tail_, tail);
}

void Pipe::Process()
{
    Dir head;
    Dir tail;
    GetTailAndHead(GetDir(), &head, &tail);

    ProcessHelper(head_, head);
    ProcessHelper(tail_, tail);
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
    Dir tail;
    Dir left;
    Dir right;
    GetConnectionsDirs(GetDir(), &tail, &left, &right);

    if (   (dir == tail)
        && (!tail_.valid()))
    {
        tail_ = pipe;
        return true;
    }

    if (   (dir == left)
        && (!left_.valid()))
    {
        left_ = pipe;
        return true;
    }

    if (   (dir == right)
        && (!right_.valid()))
    {
        right_ = pipe;
        return true;
    }

    return false;
}

void Manifold::AfterWorldCreation()
{
    PipeBase::AfterWorldCreation();

    Dir tail;
    Dir left;
    Dir right;
    GetConnectionsDirs(GetDir(), &tail, &left, &right);

    ConnectHelper(tail_, tail);
    ConnectHelper(left_, left);
    ConnectHelper(right_, right);
}

bool Manifold::CanTransferGas(Dir dir) const
{
    return true;
}

void Manifold::Process()
{
    Dir tail;
    Dir left;
    Dir right;
    GetConnectionsDirs(GetDir(), &tail, &left, &right);

    ProcessHelper(tail_, tail);
    ProcessHelper(left_, left);
    ProcessHelper(right_, right);
}

void Manifold::GetConnectionsDirs(Dir dir, Dir *tail, Dir *left, Dir *right)
{
    *tail = helpers::revert_dir(dir);
    if (dir == D_DOWN)
    {
        *left = D_RIGHT;
        *right = D_LEFT;
    }
    else if (dir == D_UP)
    {
        *left = D_LEFT;
        *right = D_RIGHT;
    }
    else if (dir == D_RIGHT)
    {
        *left = D_UP;
        *right = D_DOWN;
    }
    else
    {
        *left = D_DOWN;
        *left = D_UP;
    }
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
