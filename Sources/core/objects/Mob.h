#pragma once

#include "MessageReceiver.h"

#include "Interface.h"

class IMob: public IMessageReceiver
{
public:
    IMob(size_t id);
    DECLARE_SAVED(IMob, IMessageReceiver);
    DECLARE_GET_TYPE_ITEM(imob)
    virtual void Delete();
    virtual void cautOverMind();

    virtual void InitGUI(){}
    virtual void DeinitGUI(){}
    virtual void GenerateInterfaceForFrame();
    virtual void processGUImsg(const Message2& msg) override;
    
    virtual void ProcessPhysics() override;

    virtual InterfaceBase* GetInterface() { return nullptr; }

    virtual void CalculateVisible(std::list<point>* visible_list) { ; }
};

ADD_TO_TYPELIST(IMob);
