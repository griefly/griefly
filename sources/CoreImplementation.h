#pragma once

#include "CoreInterface.h"

namespace kv
{

class WorldImplementation : public WorldInterface
{
public:
    virtual void SaveWorld(QByteArray* data) const override;

    // TODO: Look into #360 properly
    virtual void ProcessNextTick(const QVector<Message>& messages) override;

    virtual void Represent(GrowingFrame* frame) const override;

    virtual quint32 Hash() const override;
};

class CoreImplementation : public CoreInterface
{
public:
    virtual WorldPtr CreateWorldFromSave(const QByteArray& data) override;
    virtual WorldPtr CreateWorldFromMapgen(const QByteArray& data) override;

    virtual const ObjectsMetadata& GetObjectsMetadata() const override;
};

}
