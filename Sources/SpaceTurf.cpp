#include "SpaceTurf.h"

#include "sync_random.h"

Space::Space()
{
    passable = true;
    transparent = true;
    
    SetSprite("icons/space.png"); 
    std::stringstream conv;
    size_t value = get_rand() % 25 + 1;
    conv << value;
    SetState(conv.str());
    name = "Space";
}