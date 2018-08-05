#include "WorldLoaderSaver.h"

#include <QFile>
#include <QJsonArray>

#include "Idptr.h"

#include "objects/Object.h"
#include "objects/MaterialObject.h"
#include "objects/turfs/Turf.h"
#include "objects/Tile.h"
#include "objects/mobs/Mob.h"

#include "core_headers/Mapgen.h"

#include "SaveableOperators.h"

#include "SynchronizedRandom.h"

#include "AutogenMetadata.h"

namespace kv
{
namespace world
{

void Save(const GameInterface* game, kv::FastSerializer& serializer)
{
    SaveMapHeader(game, serializer);

    auto& objects_table = game->GetFactory().GetIdTable();

    auto it = objects_table.begin();
    ++it;
    while (it != objects_table.end())
    {
        if (it->object)
        {
            it->object->Save(serializer);
        }
        ++it;
    }

    serializer.WriteType(kv::END_TYPE);
}

void Load(GameInterface* game, kv::FastDeserializer& deserializer)
{
    ObjectFactoryInterface& factory = game->GetFactory();

    factory.Clear();

    LoadMapHeader(game, deserializer);
    while (!deserializer.IsEnd())
    {
        QString type;
        deserializer.ReadType(&type);

        if (type == kv::END_TYPE)
        {
            qDebug() << "Zero id reached";
            break;
        }

        quint32 id_loc;
        deserializer >> id_loc;

        kv::Object* object = factory.CreateVoid(type, id_loc);
        object->Load(deserializer);
    }
    factory.MarkWorldAsCreated();

    game->GetAtmosphere().LoadGrid(&game->GetMap());
}

void SaveMapHeader(const GameInterface* game, kv::FastSerializer& serializer)
{
    const ObjectFactoryInterface& factory = game->GetFactory();

    serializer << factory.GetId();

    serializer << game->GetGlobals();
}

void LoadMapHeader(GameInterface* game, kv::FastDeserializer& deserializer)
{
    ObjectFactoryInterface& factory = game->GetFactory();

    int id;
    deserializer >> id;
    qDebug() << "id_: " << id;
    factory.SetId(id);

    quint32 globals;
    deserializer >> globals;
    game->SetGlobals(globals);

    factory.GetIdTable().resize(id + 1);
}

QByteArray ConvertJsonToSerialized(const QJsonValue& data)
{
    using namespace mapgen;

    kv::FastSerializer serializer(1024);

    const QJsonObject object = data.toObject();
    const QStringList keys = object.keys();
    if (keys.isEmpty())
    {
        qFatal("ConvertJsonToSerialized: no keys!");
    }
    const QString type = keys.first();
    const QJsonValue value = object.value(type);

    if (type == key::type::BOOL)
    {
        serializer << value.toBool();
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::INT32)
    {
        serializer << value.toInt();
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::UINT32)
    {
        serializer << static_cast<quint32>(value.toDouble());
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::STRING)
    {
        serializer << value.toString();
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::BYTEARRAY)
    {
        serializer << QByteArray::fromHex(value.toString().toLatin1());
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    else if (type == key::type::TYPE)
    {
        serializer.WriteType(value.toString());
        return QByteArray(serializer.GetData(), serializer.GetIndex());
    }
    qDebug() << "Unknown type:" << data;
    return QByteArray();
}

IdPtr<MaterialObject> LoadObject(GameInterface* game, const QJsonObject& data)
{
    ObjectFactoryInterface& factory = game->GetFactory();

    if (data.isEmpty())
    {
        qDebug() << "Json data is empty!";
        return 0;
    }

    const QString object_type = data.value(mapgen::key::TYPE).toString();

    IdPtr<kv::MaterialObject> object = factory.CreateImpl(object_type);
    kv::Assert(object.IsValid(), QString("Unable to cast: %1").arg(object_type));

    const auto& setters_for_types = GetVariablesForTypes();
    auto it = setters_for_types.find(object_type);
    if (it == setters_for_types.end())
    {
        qDebug() << setters_for_types.size();
        kv::Abort(QString("Unable to find setters for type: %1").arg(object_type));
    }
    const VariablesForType& setters_for_type = it->second;
    const QJsonObject variables = data.value(mapgen::key::VARIABLES).toObject();

    for (const QString& key : variables.keys())
    {
        auto it = setters_for_type.find(key);
        if (it == setters_for_type.end())
        {
            kv::Abort(
                QString("Unable to find setter for type::variable: %1::%2")
                    .arg(object_type)
                    .arg(key));
        }
        const VariableSetter& setter = it->second.setter;

        const QByteArray serialized = ConvertJsonToSerialized(variables.value(key));
        kv::FastDeserializer deserializer(serialized.data(), serialized.size());
        setter(object.operator->(), deserializer);
    }

    {
        const QString sprite = data.value(mapgen::key::SPRITE).toString();
        object->SetSprite(sprite);
    }

    {
        const QString state = data.value(mapgen::key::STATE).toString();
        object->SetState(state);
    }

    return object;
}

namespace
{

void LoadObject(GameInterface* game, const QJsonObject& data, kv::Position position, bool is_turf)
{
    auto object = world::LoadObject(game, data);
    if (!object.IsValid())
    {
        return;
    }

    auto& tile = game->GetMap().At(position.x, position.y, position.z);
    if (is_turf)
    {
        if (IdPtr<kv::Turf> turf = object)
        {
            if (tile->GetTurf())
            {
                kv::Abort(
                    QString("Double turf at %1, %2, %3").arg(position.x, position.y, position.z));
            }
            tile->SetTurf(turf);
        }
        else
        {
            kv::Abort(
                QString("Object is not turf: %1")
                    .arg(data.value(mapgen::key::TYPE).toString()));
        }
    }
    else
    {
        tile->AddObject(object);
    }
}

} // namespace

void LoadFromJsonMapGen(GameInterface* game, const QJsonObject& data)
{
    ObjectFactoryInterface& factory = game->GetFactory();
    factory.BeginWorldCreation();

    // TODO: validate json

    const int map_x = data.value(mapgen::key::WIDTH).toInt();
    const int map_y = data.value(mapgen::key::HEIGHT).toInt();
    const int map_z = data.value(mapgen::key::DEPTH).toInt();

    auto& map = game->GetMap();

    // Making tiles
    map.Resize(map_x, map_y, map_z);
    for (int x = 0; x < map.GetWidth(); x++)
    {
        for (int y = 0; y < map.GetHeight(); y++)
        {
            for (int z = 0; z < map.GetDepth(); z++)
            {
                IdPtr<CubeTile> tile = game->GetFactory().CreateImpl(CubeTile::GetTypeStatic());
                tile->SetPos({x, y, z});
                map.At(x, y, z) = tile;
            }
        }
    }

    game->GetAtmosphere().LoadGrid(&game->GetMap());

    const QJsonArray tiles = data.value(mapgen::key::TILES).toArray();

    for (const QJsonValue& tile_value : tiles)
    {
        const QJsonObject tile = tile_value.toObject();

        const int x = tile.value(mapgen::key::X).toInt();
        const int y = tile.value(mapgen::key::Y).toInt();
        const int z = tile.value(mapgen::key::Z).toInt();

        const QJsonObject turf = tile.value(mapgen::key::TURF).toObject();
        LoadObject(game, turf, {x, y, z}, true);

        const QJsonArray objects = tile.value(mapgen::key::OBJECTS).toArray();
        for (const QJsonValue& object_value : objects)
        {
            const QJsonObject object = object_value.toObject();
            LoadObject(game, object, {x, y, z}, false);
        }
    }

    factory.FinishWorldCreation();
    game->GetMap().FillTilesAtmosHolders();
}

} // namespace world
} // namespace kv
