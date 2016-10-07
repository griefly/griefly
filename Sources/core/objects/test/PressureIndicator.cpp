#include "PressureIndicator.h"

PressureIndicator::PressureIndicator(size_t id) : IOnMapObject(id)
{
    v_level = 11;
    name = "Pressure indicator";
    SetSprite("icons/numbers.dmi");
    SetState("empty");
}

void PressureIndicator::Process()
{
    SetNumber((MAIN_TICK / 10) % 100);
}

void PressureIndicator::AfterWorldCreation()
{
    IOnMapObject::AfterWorldCreation();
    SetFreq(1);
}

void PressureIndicator::SetNumber(int number)
{
    if (number >= 100)
    {
        number = 99;
    }
    if (number < 0)
    {
        number = 0;
    }

    std::string first;
    std::string second;

    // TODO: pick from array
    {
        int first_digit = number / 10;
        std::stringstream converter;
        converter << first_digit;
        converter >> first;
    }

    {
        int second_digit = number % 10;
        std::stringstream converter;
        converter << second_digit;
        converter >> second;
    }
    view_.RemoveOverlays();
    view_.AddOverlay("icons/numbers.dmi", first).SetShift(-7, 0);
    view_.AddOverlay("icons/numbers.dmi", second).SetShift(7, 0);
}
