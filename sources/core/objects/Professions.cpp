#include "Professions.h"

#include "Clothes.h"
#include "MedbayTools.h"

void professions::ToSecurityOfficer(IdPtr<Human> human)
{
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

void professions::ToDoctor(IdPtr<Human> human)
{
    HumanInterface* interface = human->GetHumanInterface();

    human->SetState("caucasian2_m_s");

    interface->uniform_.Set(human->Create<Item>(MedicalUniform::T_ITEM_S()));
    interface->feet_.Set(human->Create<Item>(WhiteShoes::T_ITEM_S()));
    interface->r_hand_.Set(human->Create<Item>(BruisePack::T_ITEM_S()));

    interface->uniform_.Get()->SetOwner(human.Id());
    interface->feet_.Get()->SetOwner(human.Id());
    interface->r_hand_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}

void professions::ToAssistant(IdPtr<Human> human)
{
    HumanInterface* interface = human->GetHumanInterface();

    human->SetState("caucasian2_m_s");

    interface->uniform_.Set(human->Create<Item>(JanitorUniform::T_ITEM_S()));
    interface->feet_.Set(human->Create<Item>(BlackBoots::T_ITEM_S()));

    // Grey uniform hack
    interface->uniform_.Get()->SetState("grey");
    interface->uniform_.Get()->name = "Grey uniform";

    interface->uniform_.Get()->SetOwner(human.Id());
    interface->feet_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}
