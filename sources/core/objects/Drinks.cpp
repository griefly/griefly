#include "Drinks.h"

Drinks::Drinks(quint32 id) : Item(id)
{
    name = "BaseDrink";
    SetSprite("icons/drinks.dmi");
    full_ = false;
}
void Drinks::Drink(IdPtr<Human> consumer, IdPtr<Human> feeder)
{
    if (!full_)
    {
        full_ = true;
        if (consumer == feeder)
        {
            GetGame().GetChat().PostTextFor(
                QString("You swallow a gulp of %1.").arg(name), consumer->GetId());
        }
        else
        {
            GetGame().GetChat().PostSimpleText(
                QString("%1 attempts to feed the contents of %2 to %3.")
                    .arg(feeder->name).arg(name).arg(consumer->name),
                consumer->GetId());
        }
        PlaySoundIfVisible("drink.ogg", consumer->GetRoot().Id());
        return;
    }
    GetGame().GetChat().PostTextFor(
        QString("%1 is empty.").arg(name), consumer->GetId());
    
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
