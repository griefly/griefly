#pragma once

#include "core/Idptr.h"

class CubeTile;
class Atmosphere;
class IMapMaster
{
public:
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetDepth() const = 0;

    typedef id_ptr_on<CubeTile> SqType;
    virtual std::vector<std::vector<std::vector<SqType>>>& GetSquares() = 0;
    virtual const std::vector<std::vector<std::vector<SqType>>>& GetSquares() const = 0;

    virtual std::list<point>* GetVisiblePoints() = 0;
    virtual Atmosphere& GetAtmosphere() = 0;
    virtual void FillAtmosphere() = 0;
    virtual void CalculateVisisble(std::list<point>* retval, int posx, int posy, int posz = 0) = 0;
};

class IObjectFactory;
class IChat;
class ITextPainter;
class ISyncRandom;
class INames;
class IMob;

class IGame
{
public:
    virtual IMapMaster& GetMap() = 0;
    virtual const IMapMaster& GetMap() const = 0;
    virtual IObjectFactory& GetFactory() = 0;
    virtual IChat& GetChat() = 0;
    virtual ITextPainter& GetTexts() = 0;
    virtual ISyncRandom& GetRandom() = 0;
    virtual INames& GetNames() = 0;

    virtual void ChangeMob(id_ptr_on<IMob> new_mob) = 0;
    virtual id_ptr_on<IMob> GetMob() = 0;

    virtual void PlayMusic(std::string name, float volume) = 0;
};
