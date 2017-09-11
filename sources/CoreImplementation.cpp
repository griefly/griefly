#include "CoreImplementation.h"
#include "CoreInterface.h"

#include "AutogenMetadata.h"
#include "core/objects/MaterialObject.h"

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

void WorldImplementation::SaveWorld(QByteArray* data) const
{
    // TODO
    Q_UNUSED(data)
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
