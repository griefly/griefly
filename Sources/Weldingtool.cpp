#include "Weldingtool.h"

Weldingtool::Weldingtool()
{
    SetState("welder");

    name = "Weldingtool";

    on_ = false;
    fuel_ = 50;
}

void Weldingtool::process()
{
    --fuel_;
    if (fuel_ <= 0)
    {
        TurnOff();
        fuel_ = 0;
    }
}

void Weldingtool::TurnOn()
{
    if (Working())
        return;
    if (fuel_ <= 0)
        return;

    on_ = true;
    SetState("welder1");
    SetFreq(10);
}

void Weldingtool::TurnOff()
{
    if (!Working())
        return;
    
    on_ = false;
    SetState("welder");
    SetFreq(0);
}

void Weldingtool::AttackBy(id_ptr_on<Item> item)
{
    if (Working())
        TurnOff();
    else
        TurnOn();
}