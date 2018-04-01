#pragma once

#include <list>

#include "KvGlobals.h"
#include "core_headers/CoreInterface.h"

#include "Idptr.h"

#include <QString>

class Names;
namespace kv
{
    class Mob;
    class MapObject;
    class Object;
    class CubeTile;
    class GlobalObjectsHolder;
    class ChatFrameInfo;
}
class MapInterface;
class Representation;

class AtmosInterface
{
public:
    virtual ~AtmosInterface() { }

    virtual void Process(qint32 game_tick) = 0;
    virtual void ProcessConsequences(qint32 game_tick) = 0;

    virtual void Represent(kv::GrowingFrame* frame) const = 0;

    using Flags = char;
    virtual void SetFlags(quint32 x, quint32 y, quint32 z, Flags flags) = 0;
    virtual void LoadGrid(MapInterface* map) = 0;
};

using VisiblePoints = QVector<kv::Position>;

class MapInterface
{
public:
    virtual ~MapInterface() { }

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetDepth() const = 0;

    virtual void Represent(kv::GrowingFrame* frame, const VisiblePoints& points, IdPtr<kv::Mob> mob) const = 0;

    virtual void Resize(int new_map_x, int new_map_y, int new_map_z) = 0;

    using SqType = IdPtr<kv::CubeTile>;
    virtual SqType& At(int x, int y, int z) = 0;
    virtual const SqType& At(int x, int y, int z) const = 0;

    virtual void FillTilesAtmosHolders() = 0;
    virtual void CalculateLos(VisiblePoints* retval, int posx, int posy, int posz = 0) const = 0;

    virtual bool Istransparent(int posx, int posy, int posz = 0) const = 0;
};

class ObjectFactoryInterface
{
public:
    virtual ~ObjectFactoryInterface() { }

    virtual QVector<ObjectInfo>& GetIdTable() = 0;
    virtual const QVector<ObjectInfo>& GetIdTable() const = 0;

    virtual quint32 Hash() const = 0;

    virtual void BeginWorldCreation() = 0;
    virtual void FinishWorldCreation() = 0;
    virtual void MarkWorldAsCreated() = 0;

    virtual quint32 CreateImpl(const QString& type, quint32 owner = 0) = 0;
    virtual kv::Object* CreateVoid(const QString& hash, quint32 id_new) = 0;

    virtual void DeleteLater(quint32 id) = 0;
    virtual void ProcessDeletion() = 0;

    virtual void Clear() = 0;

    virtual int GetId() const = 0;
    virtual void SetId(int id) = 0;
};

class ObjectProcessorInterface
{
public:
    virtual ~ObjectProcessorInterface() { }

    virtual void Add(quint32 object) = 0;
    virtual void RunProcess() = 0;
};

class GameInterface
{
public:
    virtual ~GameInterface() { }

    virtual AtmosInterface& GetAtmosphere() = 0;
    virtual const AtmosInterface& GetAtmosphere() const = 0;
    virtual MapInterface& GetMap() = 0;
    virtual const MapInterface& GetMap() const = 0;
    virtual ObjectFactoryInterface& GetFactory() = 0;
    virtual const ObjectFactoryInterface& GetFactory() const = 0;
    virtual Names& GetNames() = 0;
    virtual kv::ChatFrameInfo& GetChatFrameInfo() = 0;
    virtual const kv::ChatFrameInfo& GetChatFrameInfo() const = 0;

    virtual ObjectProcessorInterface& GetProcessor() = 0;

    virtual IdPtr<kv::GlobalObjectsHolder> GetGlobals() const = 0;
    virtual void SetGlobals(quint32 globals) = 0;

    virtual void SetPlayerId(quint32 net_id, quint32 real_id) = 0;
    virtual quint32 GetPlayerId(quint32 net_id) const = 0;
    virtual quint32 GetNetId(quint32 real_id) const = 0;

    virtual void AddSound(const QString& name, kv::Position position) = 0;
    virtual void PlayMusic(const QString& name, int volume, quint32 mob) = 0;
};
