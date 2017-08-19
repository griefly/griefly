#include "Professions.h"

#include "movable/items/Clothes.h"
#include "movable/items/MedbayTools.h"
#include "movable/items/Weapons.h"

using namespace kv;

void professions::ToSecurityOfficer(IdPtr<Human> human)
{
    IdPtr<HumanInterface> hud = human->GetHumanInterface();

    hud->InsertItem(slot::UNIFORM, human->Create<Item>(RedUniform::GetTypeStatic()));
    hud->InsertItem(slot::FEET, human->Create<Item>(OrangeBoots::GetTypeStatic()));
    hud->InsertItem(slot::HEAD, human->Create<Item>(Helmet::GetTypeStatic()));
    hud->InsertItem(slot::SUIT, human->Create<Item>(Armor::GetTypeStatic()));

    // Red boots hack
    hud->GetItem(slot::FEET)->SetState("red");
    hud->GetItem(slot::FEET)->name = "Red boots";

    human->UpdateOverlays();
}

void professions::ToDoctor(IdPtr<Human> human)
{
    IdPtr<HumanInterface> hud = human->GetHumanInterface();

    hud->InsertItem(slot::UNIFORM, human->Create<Item>(MedicalUniform::GetTypeStatic()));
    hud->InsertItem(slot::FEET, human->Create<Item>(WhiteShoes::GetTypeStatic()));
    hud->InsertItem(slot::RIGHT_HAND, human->Create<Item>(BruisePack::GetTypeStatic()));

    human->UpdateOverlays();
}

void professions::ToAssistant(IdPtr<Human> human)
{
    IdPtr<HumanInterface> hud = human->GetHumanInterface();

    hud->InsertItem(slot::UNIFORM, human->Create<Item>(JanitorUniform::GetTypeStatic()));
    hud->InsertItem(slot::FEET, human->Create<Item>(BlackBoots::GetTypeStatic()));

    // Grey uniform hack
    hud->GetItem(slot::UNIFORM)->SetState("grey");
    hud->GetItem(slot::UNIFORM)->name = "Grey uniform";

    human->UpdateOverlays();
}

void professions::ToClown(IdPtr<Human> human)
{
    IdPtr<HumanInterface> hud = human->GetHumanInterface();

    hud->InsertItem(slot::UNIFORM, human->Create<Item>(ClownUniform::GetTypeStatic()));
    hud->InsertItem(slot::FEET, human->Create<Item>(ClownBoots::GetTypeStatic()));

    human->UpdateOverlays();
}

void professions::ToBarman(IdPtr<Human> human)
{
    IdPtr<HumanInterface> hud = human->GetHumanInterface();

    hud->InsertItem(slot::UNIFORM, human->Create<Item>(BarmanUniform::GetTypeStatic()));
    hud->InsertItem(slot::FEET, human->Create<Item>(BlackBoots::GetTypeStatic()));
    hud->InsertItem(slot::HEAD, human->Create<Item>(Tophat::GetTypeStatic()));
    hud->InsertItem(slot::SUIT, human->Create<Item>(Armor::GetTypeStatic()));
    hud->InsertItem(slot::RIGHT_HAND, human->Create<Item>(Revolver::GetTypeStatic()));

    human->UpdateOverlays();
}
