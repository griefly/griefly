#include "Shard.h"

#include "../SyncRandom.h"

Shard::Shard(size_t id) : Item(id)
{
    SetSprite("icons/shards.dmi");

    unsigned int val = get_rand() % 3;
    std::string st;
    if (val == 0)
        st = "large";
    else if (val == 1)
        st = "medium";
    else
        st = "small";
    SetState(st);
    name = "Shard";
};
