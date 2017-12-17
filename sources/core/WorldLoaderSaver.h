#pragma once

#include <QJsonObject>

#include "Interfaces.h"
#include "core/FastSerializer.h"

namespace kv
{
namespace world
{

void Save(const GameInterface* game, kv::FastSerializer& serializer);
// TODO: 'real_this_mob' should be removed from here
void Load(GameInterface* game, kv::FastDeserializer& deserializer, quint32 real_this_mob);
void LoadFromJsonMapGen(GameInterface* game, const QJsonObject& data);
void SaveMapHeader(const GameInterface* game, kv::FastSerializer& serializer);
void LoadMapHeader(GameInterface* game, kv::FastDeserializer& deserializer);

}
}
