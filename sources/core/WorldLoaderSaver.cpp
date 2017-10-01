#include "WorldLoaderSaver.h"

#include <QFile>

#include "Idptr.h"

#include "objects/Object.h"
#include "objects/MaterialObject.h"
#include "objects/turfs/Turf.h"
#include "objects/Tile.h"
#include "objects/mobs/Mob.h"

#include "core/SaveableOperators.h"

#include "SynchronizedRandom.h"

#include "MapEditor.h"

#include "AutogenMetadata.h"

namespace kv
{

WorldLoaderSaver::WorldLoaderSaver(GameInterface* game)
    : game_(game)
{
    // Nothing
}

void WorldLoaderSaver::Save(kv::FastSerializer& serializer) const
{
    SaveMapHeader(serializer);

    auto& objects_table = game_->GetFactory().GetIdTable();

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

void WorldLoaderSaver::Load(kv::FastDeserializer& deserializer, quint32 real_this_mob)
{
    ObjectFactoryInterface& factory = game_->GetFactory();

    factory.Clear();

    LoadMapHeader(deserializer);
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

    IdPtr<kv::Mob> player = game_->GetPlayerId(real_this_mob);
    game_->SetMob(player.Id());
    qDebug() << "Player id:" << player.Id();
    factory.MarkWorldAsCreated();

    game_->GetAtmosphere().LoadGrid(&game_->GetMap());
}

void WorldLoaderSaver::LoadFromMapGen(const QString& name)
{
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly))
    {
        kv::Abort(QString("Error open: %1").arg(name));
    }

    QByteArray raw_data;
    while (file.bytesAvailable())
    {
        QByteArray local = file.readLine();
        if (local.size() < 1)
        {
            break;
        }
        local = local.left(local.size() - 1);
        raw_data.append(local);
    }
    raw_data = QByteArray::fromHex(raw_data);
    kv::FastDeserializer deserializer(raw_data.data(), raw_data.size());
    LoadFromMapGen(deserializer);
}

void WorldLoaderSaver::LoadFromMapGen(FastDeserializer& deserializer)
{
    ObjectFactoryInterface& factory = game_->GetFactory();
    factory.BeginWorldCreation();

    int map_x;
    int map_y;
    int map_z;

    deserializer >> map_x;
    deserializer >> map_y;
    deserializer >> map_z;

    auto& map = game_->GetMap();

    // Making tiles
    map.Resize(map_x, map_y, map_z);
    for (int x = 0; x < map.GetWidth(); x++)
    {
        for (int y = 0; y < map.GetHeight(); y++)
        {
            for (int z = 0; z < map.GetDepth(); z++)
            {
                IdPtr<CubeTile> tile = game_->GetFactory().CreateImpl(CubeTile::GetTypeStatic());
                tile->SetPos({x, y, z});
                map.At(x, y, z) = tile;
            }
        }
    }

    game_->GetAtmosphere().LoadGrid(&game_->GetMap());

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

        auto& tile = game_->GetMap().At(x, y, z);
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
    game_->GetMap().FillTilesAtmosHolders();
}

void WorldLoaderSaver::SaveMapHeader(kv::FastSerializer& serializer) const
{
    ObjectFactoryInterface& factory = game_->GetFactory();

    serializer << factory.GetId();

    serializer << game_->GetGlobals();
}

void WorldLoaderSaver::LoadMapHeader(kv::FastDeserializer& deserializer)
{
    ObjectFactoryInterface& factory = game_->GetFactory();

    int id;
    deserializer >> id;
    qDebug() << "id_: " << id;
    factory.SetId(id);

    quint32 globals;
    deserializer >> globals;
    game_->SetGlobals(globals);

    factory.GetIdTable().resize(id + 1);
}

}
