#include "Weldingtool.h"

using namespace kv;

Weldingtool::Weldingtool()
{
    SetState("welder");

    SetName("Weldingtool");

    on_ = false;
    fuel_ = 250;
}

void Weldingtool::Process()
{
    if (atmos::AtmosHolder* holder = GetRoot()->GetAtmosHolder())
    {
        holder->Ignite();
    }

    --fuel_;
    if (fuel_ <= 0)
    {
        TurnOff();
        fuel_ = 0;
    }
}

int Weldingtool::AddFuel(int amount)
{
    fuel_ += amount;
    if (fuel_ < 0)
    {
        fuel_ = 0;
    }
    if (fuel_ > 250)
    {
        fuel_ = 250;
    }
    return fuel_;
}

void Weldingtool::TurnOn()
{
    if (Working())
    {
        return;
    }
    if (fuel_ <= 0)
    {
        return;
    }

    on_ = true;
    SetState("welder1");
    SetFreq(2);
}

void Weldingtool::TurnOff()
{
    if (!Working())
    {
        return;
    }
    
    on_ = false;
    SetState("welder");
    SetFreq(0);
}

void Weldingtool::AttackBy(IdPtr<Item> item)
{
    if (Working())
    {
        TurnOff();
    }
    else
    {
        TurnOn();
    }
}
