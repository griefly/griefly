#include "Professions.h"

#include "Clothes.h"

void professions::ToSecurityOfficer(IdPtr<Human> human)
{
    if (!human.IsValid())
    {
        qDebug() << "Human is not valid in 'ToSecurityOfficer'";
        return;
    }

    HumanInterface* interface = human->GetHumanInterface();

    human->SetState("caucasian2_m_s");

    interface->uniform_.Set(human->Create<Item>(RedUniform::T_ITEM_S()));
    interface->feet_.Set(human->Create<Item>(OrangeBoots::T_ITEM_S()));
    interface->head_.Set(human->Create<Item>(Helmet::T_ITEM_S()));
    interface->suit_.Set(human->Create<Item>(Armor::T_ITEM_S()));

    // Red boots hack
    interface->feet_.Get()->SetState("red");
    interface->feet_.Get()->name = "Red boots";

    interface->uniform_.Get()->SetOwner(human.Id());
    interface->feet_.Get()->SetOwner(human.Id());
    interface->head_.Get()->SetOwner(human.Id());
    interface->suit_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}
