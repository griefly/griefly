#include "Pipes.h"

Pipe::Pipe(size_t id) : IMovable(id)
{
    anchored = true;
    v_level = 3;

    SetSprite("icons/pipes.dmi");
    SetState("intact");

    name = "Pipe";
}
