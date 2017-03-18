#include "Professions.h"

#include "Clothes.h"
#include "MedbayTools.h"
#include "Weapons.h"
#include "Interface.h"

void professions::ToSecurityOfficer(IdPtr<Human> human)
{
    HumanInterface* hud = human->GetHumanInterface();

    hud->uniform_.Set(human->Create<Item>(RedUniform::GetTypeStatic()));
    hud->feet_.Set(human->Create<Item>(OrangeBoots::GetTypeStatic()));
    hud->head_.Set(human->Create<Item>(Helmet::GetTypeStatic()));
    hud->suit_.Set(human->Create<Item>(Armor::GetTypeStatic()));

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

    hud->uniform_.Set(human->Create<Item>(MedicalUniform::GetTypeStatic()));
    hud->feet_.Set(human->Create<Item>(WhiteShoes::GetTypeStatic()));
    hud->r_hand_.Set(human->Create<Item>(BruisePack::GetTypeStatic()));

    hud->uniform_.Get()->SetOwner(human.Id());
    hud->feet_.Get()->SetOwner(human.Id());
    hud->r_hand_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}

void professions::ToAssistant(IdPtr<Human> human)
{
    HumanInterface* hud = human->GetHumanInterface();

    hud->uniform_.Set(human->Create<Item>(JanitorUniform::GetTypeStatic()));
    hud->feet_.Set(human->Create<Item>(BlackBoots::GetTypeStatic()));

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

    hud->uniform_.Set(human->Create<Item>(ClownUniform::GetTypeStatic()));
    hud->feet_.Set(human->Create<Item>(ClownBoots::GetTypeStatic()));

    hud->uniform_.Get()->SetOwner(human.Id());
    hud->feet_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}

void professions::ToBarman(IdPtr<Human> human)
{
    HumanInterface* hud = human->GetHumanInterface();

    hud->uniform_.Set(human->Create<Item>(BarmanUniform::GetTypeStatic()));
    hud->feet_.Set(human->Create<Item>(BlackBoots::GetTypeStatic()));
    hud->head_.Set(human->Create<Item>(Tophat::GetTypeStatic()));
    hud->suit_.Set(human->Create<Item>(Armor::GetTypeStatic()));
    hud->r_hand_.Set(human->Create<Item>(Revolver::GetTypeStatic()));

    hud->uniform_.Get()->SetOwner(human.Id());
    hud->feet_.Get()->SetOwner(human.Id());
    hud->head_.Get()->SetOwner(human.Id());
    hud->suit_.Get()->SetOwner(human.Id());
    hud->r_hand_.Get()->SetOwner(human.Id());

    human->UpdateOverlays();
}
