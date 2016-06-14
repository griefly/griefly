#pragma once

#include <gmock/gmock.h>

#include "Interfaces.h"

#include "representation/Chat.h"
#include "representation/Text.h"
#include "core/SyncRandom.h"
#include "core/Names.h"

class MockIGame : public IGame
{
public:
    MOCK_METHOD0(GetMap, IMapMaster&());
    MOCK_CONST_METHOD0(GetMap, const IMapMaster&());
    MOCK_METHOD0(GetFactory, IObjectFactory&());
    MOCK_METHOD0(GetChat, Chat&());
    MOCK_METHOD0(GetTexts, TextPainter&());
    MOCK_METHOD0(GetRandom, SyncRandom&());
    MOCK_METHOD0(GetNames, Names&());
    MOCK_METHOD3(MakeTiles, void(int size_x, int size_y, int size_z));
    MOCK_METHOD1(SetUnsyncGenerator, void(size_t generator));
    MOCK_METHOD1(ChangeMob, void(id_ptr_on<IMob> new_mob));
    MOCK_METHOD0(GetMob, id_ptr_on<IMob>());
    MOCK_METHOD1(SetMob, void(size_t new_mob));
    MOCK_METHOD2(PlayMusic, void(std::string name, float volume));
};

