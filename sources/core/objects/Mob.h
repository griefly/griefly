#pragma once

#include "Movable.h"

#include "Interface.h"

#include "net/Network2.h"

namespace kv
{

class IMob : public IMovable
{
public:
    DECLARE_SAVEABLE(IMob, IMovable);
    REGISTER_CLASS_AS(imob);
    IMob();
    virtual void AfterWorldCreation() override;
    virtual void Delete();

    virtual void InitGUI(){}
    virtual void DeinitGUI(){}
    virtual void GenerateInterfaceForFrame();
    virtual void ProcessMessage(const Message2& msg);

    virtual InterfaceBase* GetInterface() { return nullptr; }

    virtual void CalculateVisible(std::list<PosPoint>* visible_list) { ; }
};
END_DECLARE(IMob);

}
