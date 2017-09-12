#pragma once

#include "CoreInterface.h"
#include "Interfaces.h"
#include "core/ChatFrameInfo.h"

namespace kv
{

class WorldImplementation : public WorldInterface, public GameInterface
{
public:
    // World interface

    // TODO: Look into #360 properly
    virtual void ProcessNextTick(const QVector<Message>& messages) override;

    virtual void Represent(GrowingFrame* frame) const override;

    virtual quint32 Hash() const override;

    virtual void SaveWorld(QByteArray* data) const override;

    // Game interface
    virtual AtmosInterface& GetAtmosphere() override;
    virtual MapInterface& GetMap() override;
    virtual const MapInterface& GetMap() const override;
    virtual ObjectFactoryInterface& GetFactory() override;
    virtual Names& GetNames() override;
    virtual kv::ChatFrameInfo& GetChatFrameInfo() override;

    virtual void MakeTiles(int size_x, int size_y, int size_z) override;

    virtual IdPtr<kv::Mob> GetMob() override;
    virtual void SetMob(quint32 new_mob) override;

    virtual IdPtr<kv::GlobalObjectsHolder> GetGlobals() const override;
    virtual void SetGlobals(quint32 globals) override;

    virtual void SetPlayerId(quint32 net_id, quint32 real_id) override;
    virtual quint32 GetPlayerId(quint32 net_id) const override;
    virtual quint32 GetNetId(quint32 real_id) const override;

    virtual void AddSound(const QString& name, kv::Position position) override;
    virtual void PlayMusic(const QString& name, int volume, quint32 mob) override;
private:
    AtmosInterface* atmos_;
    ObjectFactoryInterface* factory_;
    kv::ChatFrameInfo chat_frame_info_;

    IdPtr<kv::GlobalObjectsHolder> global_objects_;
    Names* names_;
};

class CoreImplementation : public CoreInterface
{
public:
    virtual WorldPtr CreateWorldFromSave(const QByteArray& data) override;
    virtual WorldPtr CreateWorldFromMapgen(const QByteArray& data) override;

    virtual const ObjectsMetadata& GetObjectsMetadata() const override;
};

}
