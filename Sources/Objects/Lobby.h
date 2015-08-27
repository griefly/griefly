#pragma once

#include "MainInt.h"

class Lobby: public IMainObject
{
public:
    DECLARE_SAVED(Lobby, IMainObject);
    DECLARE_GET_TYPE_ITEM(Lobby)
    Lobby(size_t id);
    int GetSecondUntilStart();
    virtual void process() override;
private:
    int KV_SAVEBLE(seconds_);
    KV_ON_LOAD_CALL(SetThisAsLobby);
    void SetThisAsLobby();
};
ADD_TO_TYPELIST(Lobby);

void SetLobby(Lobby* lobby);
Lobby& GetLobby();
