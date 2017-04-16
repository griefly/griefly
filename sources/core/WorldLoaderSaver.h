#pragma once

#include "Interfaces.h"
#include "FastSerializer.h"

class WorldLoaderSaver
{
public:
    WorldLoaderSaver(IGame* game);

    void Save(FastSerializer& str);
    void Load(FastDeserializer& str, quint32 real_this_mob);
    void LoadFromMapGen(const QString& name);
private:
    void SaveMapHeader(FastSerializer& serializer);
    void LoadMapHeader(FastDeserializer& deserializer);

    IGame* game_;
};
