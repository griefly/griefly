#pragma once

#include "../MaterialObject.h"

namespace kv
{

class PressureIndicator : public MaterialObject
{
public:
    DECLARE_SAVEABLE(PressureIndicator, MaterialObject);
    REGISTER_CLASS_AS(PressureIndicator);
    PressureIndicator();
    virtual void Process() override;
    virtual void AfterWorldCreation() override;
private:
    void SetNumber(int number);
};
END_DECLARE(PressureIndicator);

}
