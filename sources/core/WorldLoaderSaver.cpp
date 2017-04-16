#include "WorldLoaderSaver.h"

WorldLoaderSaver::WorldLoaderSaver(IGame* game)
    : game_(game)
{
    // Nothing
}

void WorldLoaderSaver::Save(FastSerializer& str)
{
    // TODO
}

void WorldLoaderSaver::Load(FastDeserializer& str, quint32 real_this_mob)
{
    // TODO
}

void WorldLoaderSaver::LoadFromMapGen(const QString& name)
{
    // TODO
}

void WorldLoaderSaver::SaveMapHeader(FastSerializer& serializer)
{
    // TOOD
}

void WorldLoaderSaver::LoadMapHeader(FastDeserializer& deserializer)
{
    // TODO
}
