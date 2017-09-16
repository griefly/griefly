#pragma once

#include "Interfaces.h"
#include "FastSerializer.h"

namespace kv
{

class WorldLoaderSaver
{
public:
    WorldLoaderSaver(GameInterface* game);

    void Save(kv::FastSerializer& serializer) const;
    // TODO: 'real_this_mob' should be removed from here
    void Load(kv::FastDeserializer& deserializer, quint32 real_this_mob);
    void LoadFromMapGen(const QString& name);
private:
    void SaveMapHeader(kv::FastSerializer& serializer) const;
    void LoadMapHeader(kv::FastDeserializer& deserializer);

    GameInterface* game_;
};

}
