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

#include "core/SaveableOperators.h"

#include "SynchronizedRandom.h"

#include "MapEditor.h"

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

void Load(GameInterface* game, kv::FastDeserializer& deserializer, quint32 real_this_mob)
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

    IdPtr<kv::Mob> player = game->GetPlayerId(real_this_mob);
    game->SetMob(player.Id());
    qDebug() << "Player id:" << player.Id();
    factory.MarkWorldAsCreated();

    game->GetAtmosphere().LoadGrid(&game->GetMap());
}

void LoadFromMapGen(GameInterface* game, FastDeserializer& deserializer)
{
    ObjectFactoryInterface& factory = game->GetFactory();
    factory.BeginWorldCreation();

    int map_x;
    int map_y;
    int map_z;

    deserializer >> map_x;
    deserializer >> map_y;
    deserializer >> map_z;

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

    qDebug() << "Begin loading cycle";
    while (!deserializer.IsEnd())
    {
        QString item_type;
        qint32 x;
        qint32 y;
        qint32 z;

        deserializer.ReadType(&item_type);

        deserializer >> x;
        deserializer >> y;
        deserializer >> z;

        IdPtr<kv::MaterialObject> i = factory.CreateImpl(item_type);

        if (!i.IsValid())
        {
            kv::Abort(QString("Unable to cast: %1").arg(item_type));
        }

        MapgenVariablesType variables;
        deserializer >> variables;

        for (auto it = variables.begin(); it != variables.end(); ++it)
        {
            if ((it.value().size() == 0) || (it.key().size() == 0))
            {
                continue;
            }

            QByteArray variable_data = it.value();

            kv::FastDeserializer local(variable_data.data(), variable_data.size());

            auto& setters_for_type = GetSettersForTypes();

            setters_for_type[item_type][it.key()](i.operator->(), local);
        }

        auto& tile = game->GetMap().At(x, y, z);
        if (IdPtr<kv::Turf> turf = i)
        {
            if (tile->GetTurf())
            {
                kv::Abort(QString("Double turf at %1, %2, %3").arg(x, y, z));
            }
            tile->SetTurf(turf);
        }
        else
        {
            tile->AddObject(i);
        }
    }

    factory.FinishWorldCreation();
    game->GetMap().FillTilesAtmosHolders();
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

        const QJsonObject turf = tile.value(mapgen::key::TURF).toObject();
        const QJsonArray objects = tile.value(mapgen::key::OBJECTS).toArray();

        // TODO:
    }
}

}
}
