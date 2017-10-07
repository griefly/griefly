#pragma once

#include "Interfaces.h"
#include "FastSerializer.h"

namespace kv
{

class WorldLoaderSaver
{
public:
    WorldLoaderSaver();

    void static Save(const GameInterface* game, kv::FastSerializer& serializer);
    // TODO: 'real_this_mob' should be removed from here
    void static Load(GameInterface* game, kv::FastDeserializer& deserializer, quint32 real_this_mob);
    void static LoadFromMapGen(GameInterface* game, FastDeserializer& deserializer);
private:
    void static SaveMapHeader(const GameInterface* game, kv::FastSerializer& serializer);
    void static LoadMapHeader(GameInterface* game, kv::FastDeserializer& deserializer);
};

}
