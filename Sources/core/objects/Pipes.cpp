#include "Pipes.h"

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
