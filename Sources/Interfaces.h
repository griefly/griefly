#pragma once

#include "core/Idptr.h"

class IMainObject;
class CubeTile;
class IOnMapBase;
class IAtmosphere
{
public:
    virtual void Resize(size_t x, size_t y, size_t z) = 0;
    virtual void Process() = 0;
    virtual void ProcessMove() = 0;
};

class IMapMaster
{
public:
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
    virtual size_t CreateImpl(const std::string& type, id_ptr_on<IOnMapBase> owner = 0) = 0;
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
    virtual IMapMaster& GetMap() = 0;
    virtual const IMapMaster& GetMap() const = 0;
    virtual ObjectFactory& GetFactory() = 0;
    virtual Chat& GetChat() = 0;
    virtual TextPainter& GetTexts() = 0;
    virtual SyncRandom& GetRandom() = 0;
    virtual Names& GetNames() = 0;

    virtual void SetUnsyncGenerator(size_t generator) = 0;

    virtual void ChangeMob(id_ptr_on<IMob> new_mob) = 0;
    virtual id_ptr_on<IMob> GetMob() = 0;

    virtual void PlayMusic(std::string name, float volume) = 0;
};
