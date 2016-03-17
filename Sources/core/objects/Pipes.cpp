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
    return false;
}
