#include "PressureIndicator.h"

PressureIndicator::PressureIndicator(size_t id) : IOnMapObject(id)
{
    v_level = 11;
    name = "Pressure indicator";
    SetSprite("icons/numbers.dmi");
    SetState("empty");
    view_.AddOverlay("icons/numbers.dmi", "1").SetShift(-5, 0);
    view_.AddOverlay("icons/numbers.dmi", "4").SetShift(5, 0);
}

void PressureIndicator::Process()
{
    // TODO
}
