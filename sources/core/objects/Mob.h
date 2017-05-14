#pragma once

#include "MessageReceiver.h"

#include "Interface.h"

namespace kv
{

class IMob : public IMessageReceiver
{
public:
    DECLARE_SAVEABLE(IMob, IMessageReceiver);
    REGISTER_CLASS_AS(imob);
    IMob();
    virtual void AfterWorldCreation() override;
    virtual void Delete();
    virtual void cautOverMind();

    virtual void InitGUI(){}
    virtual void DeinitGUI(){}
    virtual void GenerateInterfaceForFrame();
    virtual void ProcessMessage(const Message2& msg) override;

    virtual InterfaceBase* GetInterface() { return nullptr; }

    virtual void CalculateVisible(std::list<PosPoint>* visible_list) { ; }
};
END_DECLARE(IMob);

}
