#pragma once

#include "../OnMapObject.h"

namespace kv
{

class PressureIndicator : public IOnMapObject
{
public:
    DECLARE_SAVEABLE(PressureIndicator, IOnMapObject);
    REGISTER_CLASS_AS(PressureIndicator);
    PressureIndicator();
    virtual void Process() override;
    virtual void AfterWorldCreation() override;
private:
    void SetNumber(int number);
};
END_DECLARE(PressureIndicator);

}
