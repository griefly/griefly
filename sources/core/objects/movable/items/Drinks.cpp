#include "Drinks.h"

using namespace kv;

Drinks::Drinks()
{
    SetName("BaseDrink");
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
            PostHtmlFor(QString("You swallow a gulp of %1.").arg(GetName()), consumer);
        }
        else
        {
            PostVisible(
                QString("%1 attempts to feed the contents of %2 to %3.")
                    .arg(feeder->GetName()).arg(GetName()).arg(consumer->GetName()),
                consumer->GetPosition());
        }
        PlaySoundIfVisible("drink.wav");
        return;
    }
    PostHtmlFor(QString("%1 is empty.").arg(GetName()), consumer);
}

Vodka::Vodka()
{
    SetName("Vodka");
    SetState("vodkabottle");
}

Beer::Beer()
{
    SetName("Beer");
    SetState("beer");
}

Tea::Tea()
{
    SetName("Tea");
    SetState("tea");
}
