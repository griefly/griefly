#pragma once

#include <list>

#include "core/Constheader.h"
#include "core/Idptr.h"

#include <QString>

class SyncRandom;
class Names;
class IMob;
class TextPainter;
class IMainObject;
class CubeTile;
class IOnMapBase;

class IAtmosphere
{
public:
    virtual ~IAtmosphere() { }

    virtual void Resize(quint32 x, quint32 y, quint32 z) = 0;
    virtual void Process() = 0;
    virtual void ProcessMove() = 0;

    typedef char Flags;
    virtual void SetFlags(quint32 x, quint32 y, quint32 z, Flags flags) = 0;
    virtual void LoadGrid() = 0;
};

class IMap
{
public:
    virtual ~IMap() { }

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetDepth() const = 0;

    virtual void Represent() = 0;

    virtual void Resize(int new_map_x, int new_map_y, int new_map_z) = 0;

    typedef IdPtr<CubeTile> SqType;
    virtual QVector<QVector<QVector<SqType>>>& GetSquares() = 0;
    virtual const QVector<QVector<QVector<SqType>>>& GetSquares() const = 0;

    virtual std::list<PosPoint>* GetVisiblePoints() = 0;
    virtual IAtmosphere& GetAtmosphere() = 0;
    virtual void FillAtmosphere() = 0;
    virtual void CalculateVisisble(std::list<PosPoint>* retval, int posx, int posy, int posz = 0) = 0;
    virtual bool IsTileVisible(quint32 tile_id) = 0;

    virtual bool IsTransparent(int posx, int posy, int posz = 0) = 0;

    virtual bool CheckBorders(const int* x, const int* y, const int* z) const = 0;
};

class IObjectFactory
{
public:
    virtual ~IObjectFactory() { }

    virtual QVector<ObjectInfo>& GetIdTable() = 0;

    virtual void ForeachProcess() = 0;

    virtual unsigned int Hash() = 0;

    virtual void BeginWorldCreation() = 0;
    virtual void FinishWorldCreation() = 0;
    virtual void MarkWorldAsCreated() = 0;

    virtual quint32 CreateImpl(const QString& type, quint32 owner = 0) = 0;
    virtual IMainObject* CreateVoid(const QString& hash, quint32 id_new) = 0;

    virtual void DeleteLater(quint32 id) = 0;
    virtual void ProcessDeletion() = 0;

    virtual void AddProcessingItem(quint32 item) = 0;

    virtual void Clear() = 0;

    virtual int GetId() = 0;
    virtual void SetId(int id) = 0;
};

class IChat
{
public:
    virtual ~IChat() { }

    virtual void PostTextFor(const QString& str, IdPtr<IOnMapBase> owner) = 0;
    virtual void PostHtmlFor(const QString& str, IdPtr<IOnMapBase> owner) = 0;
    virtual void PostText(const QString& str) = 0;
    virtual void PostOOCText(const QString& who, const QString& str) = 0;
    virtual void PostSimpleText(const QString& str, quint32 tile_id) = 0;
    virtual void PostHtmlText(const QString& str, quint32 tile_id) = 0;
    virtual void PostDamage(const QString& by, const QString& who,
                            const QString& object, quint32 tile_id) = 0;
    virtual void PostWords(const QString& who, const QString& text, quint32 tile_id) = 0;
};

class IGame
{
public:
    virtual ~IGame() { }

    virtual IMap& GetMap() = 0;
    virtual const IMap& GetMap() const = 0;
    virtual IObjectFactory& GetFactory() = 0;
    virtual IChat& GetChat() = 0;
    virtual TextPainter& GetTexts() = 0;
    virtual SyncRandom& GetRandom() = 0;
    virtual Names& GetNames() = 0;

    virtual void MakeTiles(int size_x, int size_y, int size_z) = 0;

    virtual void SetUnsyncGenerator(quint32 generator) = 0;

    virtual void ChangeMob(IdPtr<IMob> new_mob) = 0;
    virtual IdPtr<IMob> GetMob() = 0;
    virtual void SetMob(quint32 new_mob) = 0;

    virtual void SetPlayerId(quint32 net_id, quint32 real_id) = 0;
    virtual quint32 GetPlayerId(quint32 net_id) = 0;
    virtual quint32 GetNetId(quint32 real_id) = 0;
    virtual const std::map<quint32, quint32>& GetPlayersTable() const = 0;

    virtual void AddSound(const QString& name) = 0;
    virtual void PlayMusic(const QString& name, float volume) = 0;
};
