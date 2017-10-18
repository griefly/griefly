#pragma once

#include "Interfaces.h"
#include "FastSerializer.h"

namespace kv
{
namespace world
{

void Save(const GameInterface* game, kv::FastSerializer& serializer);
// TODO: 'real_this_mob' should be removed from here
void Load(GameInterface* game, kv::FastDeserializer& deserializer, quint32 real_this_mob);
void LoadFromMapGen(GameInterface* game, FastDeserializer& deserializer);
void SaveMapHeader(const GameInterface* game, kv::FastSerializer& serializer);
void LoadMapHeader(GameInterface* game, kv::FastDeserializer& deserializer);

}
}
