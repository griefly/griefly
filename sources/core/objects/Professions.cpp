#include "Professions.h"

#include "Clothes.h"
#include "MedbayTools.h"
#include "Weapons.h"
#include "Interface.h"

void professions::ToSecurityOfficer(IdPtr<Human> human)
{
    HumanInterface* hud = human->GetHumanInterface();

    hud->uniform_.Set(human->Create<Item>(RedUniform::T_ITEM_S()));
    hud->feet_.Set(human->Create<Item>(OrangeBoots::T_ITEM_S()));
    hud->head_.Set(human->Create<Item>(Helmet::T_ITEM_S()));
    hud->suit_.Set(human->Create<Item>(Armor::T_ITEM_S()));

    // Red boots hack
    hud->feet_.Get()->SetState("red");
    hud->feet_.Get()->name = "Red boots";

    hud->uniform_.Get()->SetOwner(human.Id());
    hud->feet_.Get()->SetOwner(human.Id());
    hud->head_.Get()->SetOwner(human.Id());
    hud->suit_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}

void professions::ToDoctor(IdPtr<Human> human)
{
    HumanInterface* hud = human->GetHumanInterface();

    hud->uniform_.Set(human->Create<Item>(MedicalUniform::T_ITEM_S()));
    hud->feet_.Set(human->Create<Item>(WhiteShoes::T_ITEM_S()));
    hud->r_hand_.Set(human->Create<Item>(BruisePack::T_ITEM_S()));

    hud->uniform_.Get()->SetOwner(human.Id());
    hud->feet_.Get()->SetOwner(human.Id());
    hud->r_hand_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}

void professions::ToAssistant(IdPtr<Human> human)
{
    HumanInterface* hud = human->GetHumanInterface();

    hud->uniform_.Set(human->Create<Item>(JanitorUniform::T_ITEM_S()));
    hud->feet_.Set(human->Create<Item>(BlackBoots::T_ITEM_S()));

    // Grey uniform hack
    hud->uniform_.Get()->SetState("grey");
    hud->uniform_.Get()->name = "Grey uniform";

    hud->uniform_.Get()->SetOwner(human.Id());
    hud->feet_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}

void professions::ToClown(IdPtr<Human> human)
{
    HumanInterface* hud = human->GetHumanInterface();

    hud->uniform_.Set(human->Create<Item>(ClownUniform::T_ITEM_S()));
    hud->feet_.Set(human->Create<Item>(ClownBoots::T_ITEM_S()));

    hud->uniform_.Get()->SetOwner(human.Id());
    hud->feet_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}

void professions::ToBarman(IdPtr<Human> human)
{
    HumanInterface* hud = human->GetHumanInterface();

    hud->uniform_.Set(human->Create<Item>(BarmanUniform::T_ITEM_S()));
    hud->feet_.Set(human->Create<Item>(BlackBoots::T_ITEM_S()));
    hud->head_.Set(human->Create<Item>(Tophat::T_ITEM_S()));
    hud->suit_.Set(human->Create<Item>(Armor::T_ITEM_S()));
    hud->r_hand_.Set(human->Create<Item>(Revolver::T_ITEM_S()));

    hud->uniform_.Get()->SetOwner(human.Id());
    hud->feet_.Get()->SetOwner(human.Id());
    hud->head_.Get()->SetOwner(human.Id());
    hud->suit_.Get()->SetOwner(human.Id());
    hud->r_hand_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}
