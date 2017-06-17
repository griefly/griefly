#include "WorldLoaderSaver.h"

#include <QFile>

#include "Idptr.h"

#include "objects/Object.h"
#include "objects/MaterialObject.h"
#include "objects/turfs/Turf.h"
#include "objects/Tile.h"
#include "objects/mobs/Mob.h"

#include "SynchronizedRandom.h"

#include "MapEditor.h"

#include "AutogenMetadata.h"

WorldLoaderSaver::WorldLoaderSaver(GameInterface* game)
    : game_(game)
{
    // Nothing
}

void WorldLoaderSaver::Save(FastSerializer& serializer)
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

    serializer.WriteType(END_TYPE);
}

void WorldLoaderSaver::Load(FastDeserializer& deserializer, quint32 real_this_mob)
{
    ObjectFactoryInterface& factory = game_->GetFactory();

    factory.Clear();

    LoadMapHeader(deserializer);
    while (!deserializer.IsEnd())
    {
        QString type;
        deserializer.ReadType(&type);

        if (type == END_TYPE)
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
    ObjectFactoryInterface& factory = game_->GetFactory();

    QFile file(name);
    if (!file.open(QIODevice::ReadOnly))
    {
        KvAbort(QString("Error open: %1").arg(name));
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
    FastDeserializer ss(raw_data.data(), raw_data.size());

    factory.BeginWorldCreation();

    int x;
    int y;
    int z;

    ss >> x;
    ss >> y;
    ss >> z;

    game_->MakeTiles(x, y, z);
    game_->GetAtmosphere().LoadGrid(&game_->GetMap());

    qDebug() << "Begin loading cycle";
    while (!ss.IsEnd())
    {
        QString item_type;
        qint32 x;
        qint32 y;
        qint32 z;

        ss.ReadType(&item_type);

        ss >> x;
        ss >> y;
        ss >> z;

        IdPtr<kv::MaterialObject> i = factory.CreateImpl(item_type);
        if (!i.IsValid())
        {
            KvAbort(QString("Unable to cast: %1").arg(item_type));
        }

        MapgenVariablesType variables;
        WrapReadMessage(ss, variables);

        for (auto it = variables.begin(); it != variables.end(); ++it)
        {
            if ((it->second.size() == 0) || (it->first.size() == 0))
            {
                continue;
            }

            QByteArray variable_data = it->second;

            FastDeserializer local(variable_data.data(), variable_data.size());

            auto& setters_for_type = GetSettersForTypes();
            setters_for_type[item_type][it->first](i.operator->(), local);
        }

        auto& tile = game_->GetMap().At(x, y, z);
        if (IdPtr<kv::Turf> turf = i)
        {
            if (tile->GetTurf())
            {
                KvAbort(QString("Double turf at %1, %2, %3").arg(x, y, z));
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

void WorldLoaderSaver::SaveMapHeader(FastSerializer& serializer)
{
    ObjectFactoryInterface& factory = game_->GetFactory();

    serializer << factory.GetId();

    serializer << game_->GetGlobals();
}

void WorldLoaderSaver::LoadMapHeader(FastDeserializer& deserializer)
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
