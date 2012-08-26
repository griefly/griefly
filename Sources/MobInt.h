#pragma once

#include "MessageReceiver.h"

class IMob: public IMessageReceiver
{
public:
    IMob();
    DECLARE_GET_TYPE_ITEM(imob)
    DECLARE_SAVED(IMob, IMessageReceiver);
    virtual void delThis();
    virtual void cautOverMind();

    virtual void InitGUI(){};
    virtual void DeinitGUI(){};
    virtual void processGUI();//draw
    virtual void processGUImsg(std::string& msg) override;
    
    virtual bool checkMove(Dir direct) override;
    bool KV_SAVEBLE(onMobControl);
    bool KV_ON_LOAD(thisMobControl, false);
};

ADD_TO_TYPELIST(IMob);