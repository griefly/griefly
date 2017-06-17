#include "Shard.h"

#include "core/SyncRandom.h"

using namespace kv;

Shard::Shard()
{
    SetSprite("icons/shards.dmi");
    SetState("medium");
    name = "Shard";
}

void Shard::AfterWorldCreation()
{
    unsigned int val = GenerateRandom() % 3;
    QString st;
    if (val == 0)
    {
        st = "large";
    }
    else if (val == 1)
    {
        st = "medium";
    }
    else
    {
        st = "small";
    }
    SetState(st);
    name = "Shard";
}
