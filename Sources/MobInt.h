#pragma once

#include "MessageReceiver.h"

#include "Interface.h"

class IMob: public IMessageReceiver
{
public:
    IMob(size_t id);
    DECLARE_GET_TYPE_ITEM(imob)
    DECLARE_SAVED(IMob, IMessageReceiver);
    virtual void delThis();
    virtual void cautOverMind();

    virtual void InitGUI(){};
    virtual void DeinitGUI(){};
    virtual void processGUI();//draw
    virtual void processGUImsg(const Message& msg) override;
    
    virtual void processPhysics() override;

    virtual InterfaceBase* GetInterface() { return nullptr; };

    virtual void CalculateVisible(std::list<point>* visible_list) { ; }

    virtual bool checkMove(Dir direct) override;
    bool KV_SAVEBLE(onMobControl);
    bool KV_ON_LOAD(thisMobControl, false);
};

ADD_TO_TYPELIST(IMob);
