#pragma once

#include "Interfaces.h"
#include "FastSerializer.h"

class WorldLoaderSaver
{
public:
    WorldLoaderSaver(IGame* game);

    void Save(FastSerializer& serializer);
    // TODO: 'real_this_mob' should be removed from here
    void Load(FastDeserializer& deserializer, quint32 real_this_mob);
    void LoadFromMapGen(const QString& name);
private:
    void SaveMapHeader(FastSerializer& serializer);
    void LoadMapHeader(FastDeserializer& deserializer);

    IGame* game_;
};
