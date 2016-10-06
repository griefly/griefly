#pragma once

#include "../OnMapObject.h"

class PressureIndicator : public IOnMapObject
{
public:
    DECLARE_SAVED(PressureIndicator, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(PressureIndicator);
    PressureIndicator(size_t id);
    virtual void Process() override;
};
ADD_TO_TYPELIST(PressureIndicator);
