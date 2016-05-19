#pragma once

#include "core/Idptr.h"

class IMapMaster
{
public:
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetDepth() const = 0;
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
