#pragma once

#include <QJsonObject>

#include "Interfaces.h"
#include "FastSerializer.h"

namespace kv
{
namespace world
{

void Save(const GameInterface* game, kv::FastSerializer& serializer);
void Load(GameInterface* game, kv::FastDeserializer& deserializer);
void LoadFromJsonMapGen(GameInterface* game, const QJsonObject& data);
void SaveMapHeader(const GameInterface* game, kv::FastSerializer& serializer);
void LoadMapHeader(GameInterface* game, kv::FastDeserializer& deserializer);

QByteArray ConvertJsonToSerialized(const QJsonValue& data);

}
}
