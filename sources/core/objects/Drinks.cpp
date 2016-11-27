#include "Drinks.h"

Drinks::Drinks(quint32 id) : Item(id)
{
    name = "BaseDrink";
    SetSprite("icons/drinks.dmi");
    full_ = false;
}
void Drinks::Drink(IdPtr<Human> human1, IdPtr<Human> human2)
{
    if(!full_)
    {
        full_ = true;
        if(human1 == human2)
        {
            GetGame().GetChat().PostTextFor(
            "You swallow a gulp of " + name + ".", human1->GetId());
        }
        else
        {
            GetGame().GetChat().PostSimpleText(
            human2->name + " attempts to feed the contents of " + name + " to " + human1->name + ".", human1->GetId());
        }
        PlaySoundIfVisible("drink.ogg", human1->GetRoot().Id());
    }
}
Vodka::Vodka(quint32 id) : Drinks(id)
{
    name = "Vodka";
    SetState("vodkabottle");
}
Beer::Beer(quint32 id) : Drinks(id)
{
    name = "Beer";
    SetState("beer");
}
Tea::Tea(quint32 id) : Drinks(id)
{
    name = "Tea";
    SetState("tea");
}
