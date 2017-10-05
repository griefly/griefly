#pragma once

#include "Interfaces.h"
#include "FastSerializer.h"

namespace kv
{

class WorldLoaderSaver
{
public:
    WorldLoaderSaver();

    void Save(const GameInterface* game, kv::FastSerializer& serializer) const;
    // TODO: 'real_this_mob' should be removed from here
    void Load(GameInterface* game, kv::FastDeserializer& deserializer, quint32 real_this_mob);
    void LoadFromMapGen(GameInterface* game, FastDeserializer& deserializer);
private:
    void SaveMapHeader(const GameInterface* game, kv::FastSerializer& serializer) const;
    void LoadMapHeader(GameInterface* game, kv::FastDeserializer& deserializer);
};

}
