#include "CoreImplementation.h"
#include "CoreInterface.h"

#include "AutogenMetadata.h"

#include "core/objects/MaterialObject.h"
#include "core/objects/GlobalObjectsHolder.h"
#include "core/Map.h"

#include "core/atmos/Atmos.h"
#include "core/ObjectFactory.h"
#include "core/Names.h"

namespace
{

kv::CoreInterface::ObjectsMetadata GenerateMetadata()
{
    kv::CoreInterface::ObjectsMetadata retval;

    for (auto it : (*GetItemsCreators()))
    {
        std::unique_ptr<kv::Object> object(it.second());
        kv::MaterialObject* material = CastTo<kv::MaterialObject>(object.get());
        if (!material)
        {
            qDebug() << QString("Type '%1' is not material object!").arg(it.first);
            continue;
        }

        kv::CoreInterface::ObjectMetadata metadata;
        metadata.name = it.first;
        metadata.default_view = *(material->GetView());

        retval.insert(metadata.name, metadata);
    }

    return retval;
}

}

namespace kv
{

WorldImplementation::WorldImplementation()
    : atmos_(new Atmosphere),
      factory_(new ObjectFactory(this)),
      names_(new Names(this)),
      loader_saver_(this)
{
    // Nothing
}

WorldImplementation::~WorldImplementation()
{
    // Nothing
}

void WorldImplementation::SaveWorld(FastSerializer* data) const
{
    loader_saver_.Save(*data);
}

// TODO: Look into #360 properly
void WorldImplementation::ProcessNextTick(const QVector<Message>& messages)
{
    // TODO
    Q_UNUSED(messages)
}

void WorldImplementation::Represent(GrowingFrame* frame) const
{
    // TODO
    Q_UNUSED(frame)
}
quint32 WorldImplementation::Hash() const
{
    // TODO
    return 0;
}

AtmosInterface& WorldImplementation::GetAtmosphere()
{
    return *atmos_;
}

MapInterface& WorldImplementation::GetMap()
{
    return *(global_objects_->map);
}

const MapInterface& WorldImplementation::GetMap() const
{
    return *(global_objects_->map);
}

ObjectFactoryInterface& WorldImplementation::GetFactory()
{
    return *factory_;
}

Names& WorldImplementation::GetNames()
{
    return *names_;
}

ChatFrameInfo& WorldImplementation::GetChatFrameInfo()
{
    return chat_frame_info_;
}

void WorldImplementation::MakeTiles(int size_x, int size_y, int size_z)
{
    // TODO
}

IdPtr<Mob> WorldImplementation::GetMob()
{
    // TODO
    return 0;
}

void WorldImplementation::SetMob(quint32 new_mob)
{
    // TODO
}

IdPtr<GlobalObjectsHolder> WorldImplementation::GetGlobals() const
{
    return global_objects_;
}

void WorldImplementation::SetGlobals(quint32 globals)
{
    // TODO
}

void WorldImplementation::SetPlayerId(quint32 net_id, quint32 real_id)
{
    // TODO
}

quint32 WorldImplementation::GetPlayerId(quint32 net_id) const
{
    // TODO
    return 0;
}

quint32 WorldImplementation::GetNetId(quint32 real_id) const
{
    // TODO
    return 0;
}

void WorldImplementation::AddSound(const QString& name, Position position)
{
    // TODO
}

void WorldImplementation::PlayMusic(const QString& name, int volume, quint32 mob)
{
    // TODO
}

CoreImplementation::WorldPtr CoreImplementation::CreateWorldFromSave(const QByteArray& data)
{
    // TODO
    return nullptr;
}
CoreImplementation::WorldPtr CoreImplementation::CreateWorldFromMapgen(const QByteArray& data)
{
    // TODO
    return nullptr;
}

const CoreImplementation::ObjectsMetadata& CoreImplementation::GetObjectsMetadata() const
{
    static const ObjectsMetadata metadata = GenerateMetadata();
    return metadata;
}

CoreInterface& GetCoreInstance()
{
    static CoreImplementation core;
    return core;
}

}
