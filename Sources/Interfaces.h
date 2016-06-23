#pragma once

#include "core/Idptr.h"


// TODO: Do not forget: virtual destructors everywhere

class IMainObject;
class CubeTile;
class IOnMapBase;
class IAtmosphere
{
public:
    virtual ~IAtmosphere() { }

    virtual void Resize(size_t x, size_t y, size_t z) = 0;
    virtual void Process() = 0;
    virtual void ProcessMove() = 0;
};

class IMapMaster
{
public:
    virtual ~IMapMaster() { }

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetDepth() const = 0;

    virtual void Represent() = 0;

    virtual void ResizeMap(int new_map_x, int new_map_y, int new_map_z) = 0;

    typedef id_ptr_on<CubeTile> SqType;
    virtual std::vector<std::vector<std::vector<SqType>>>& GetSquares() = 0;
    virtual const std::vector<std::vector<std::vector<SqType>>>& GetSquares() const = 0;

    virtual std::list<point>* GetVisiblePoints() = 0;
    virtual IAtmosphere& GetAtmosphere() = 0;
    virtual void FillAtmosphere() = 0;
    virtual void CalculateVisisble(std::list<point>* retval, int posx, int posy, int posz = 0) = 0;
    virtual bool IsTileVisible(size_t tile_id) = 0;

    virtual bool IsTransparent(int posx, int posy, int posz = 0) = 0;

    virtual bool CheckBorders(const int* x, const int* y, const int* z) const = 0;
};

class IObjectFactory
{
public:
    virtual ~IObjectFactory() { }

    virtual unsigned int GetLastHash() = 0;
    virtual std::vector<IMainObject*>& GetIdTable() = 0;

    virtual void ForeachProcess() = 0;

    virtual unsigned int Hash() = 0;

    virtual void Save(std::stringstream& str) = 0;
    virtual void Load(std::stringstream& str, size_t real_this_mob) = 0;

    virtual void LoadFromMapGen(const std::string& name) = 0;

    virtual void BeginWorldCreation() = 0;
    virtual void FinishWorldCreation() = 0;

    virtual size_t CreateImpl(const std::string& type, id_ptr_on<IOnMapBase> owner = 0) = 0;

    virtual void DeleteLater(size_t id) = 0;
    virtual void ProcessDeletion() = 0;

    virtual void AddProcessingItem(id_ptr_on<IMainObject> item) = 0;

    virtual void ClearProcessing() = 0;

    virtual void SetPlayerId(size_t net_id, size_t real_id) = 0;
    virtual size_t GetPlayerId(size_t net_id) = 0;
    virtual size_t GetNetId(size_t real_id) = 0;
};

class IChat;
class ITextPainter;
class ISyncRandom;
class INames;
class IMob;

// TODO: return interfaces
class Chat;
class TextPainter;
class SyncRandom;
class Names;
class Mob;

class IGame
{
public:
    virtual ~IGame() { }

    virtual IMapMaster& GetMap() = 0;
    virtual const IMapMaster& GetMap() const = 0;
    virtual IObjectFactory& GetFactory() = 0;
    virtual Chat& GetChat() = 0;
    virtual TextPainter& GetTexts() = 0;
    virtual SyncRandom& GetRandom() = 0;
    virtual Names& GetNames() = 0;

    virtual void MakeTiles(int size_x, int size_y, int size_z) = 0;

    virtual void SetUnsyncGenerator(size_t generator) = 0;

    virtual void ChangeMob(id_ptr_on<IMob> new_mob) = 0;
    virtual id_ptr_on<IMob> GetMob() = 0;
    virtual void SetMob(size_t new_mob) = 0;

    virtual void PlayMusic(std::string name, float volume) = 0;
};
