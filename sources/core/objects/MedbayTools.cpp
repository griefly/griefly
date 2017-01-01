#include "MedbayTools.h"

HealthAnalyzer::HealthAnalyzer(quint32 id) : Item(id)
{
    SetSprite("icons/device.dmi");
    SetState("health");
    name = "Health Analyzer";
}

void HealthAnalyzer::Scan(IdPtr<Human> target)
{
    float health = target->GetHealth() / 100;
    float suffocation_damage = target->GetSuffocationDamage() / 100;
    float burn = target->GetBurnDamage() / 100;
    float brute = target->GetBruteDamage() / 100;

    GetGame().GetChat().PostTextFor(QString("Analyzing results for %1:\nOverall status:").arg(target->name), GetOwner());

    GetGame().GetChat().PostTextFor(target->IsDead() ? QString("Deceased") : QString("%1 healthy").arg(health), GetOwner());
    if(target->GetBruteDamage() > 1000)
    {
		GetGame().GetChat().PostTextFor(QString("%1 tissue damage detected.").arg(target->GetBruteDamage() > 5000 ? "Severe" : "Minor"), GetOwner());
	}
    if(target->GetBurnDamage() > 1000)
    {
        GetGame().GetChat().PostTextFor(QString("%1 burn damage detected.").arg(target->GetBurnDamage() > 5000 ? "Severe" : "Minor"), GetOwner());
	}
    if(target->GetSuffocationDamage() > 1000)
    {
        GetGame().GetChat().PostTextFor(QString("%1 oxygen deprivation detected.").arg(target->GetSuffocationDamage() > 5000 ? "Severe" : "Minor"), GetOwner());
    }
    GetGame().GetChat().PostTextFor(QString("Damage: Brute-Burn-Suffocation Specfics: %1-%2-%3").arg(brute).arg(burn).arg(suffocation_damage), GetOwner());
}
Medicine::Medicine(quint32 id) : Item(id)
{
    SetSprite("icons/item.dmi");
    burn_heal_ = 0;
    brute_heal_ = 0;
}

void Medicine::Heal(IdPtr<Human> target)
{
    if(target->GetBurnDamage() < burn_heal_)
    {
        target->AddBurnDamage(-(target->GetBurnDamage()));
        return;
    }
    if(target->GetBurnDamage() < brute_heal_)
    {
        target->AddBruteDamage(-(target->GetBruteDamage()));
        return;
    }
    target->AddBurnDamage(-(burn_heal_));
    target->AddBruteDamage(-(brute_heal_));
}

Ointment::Ointment(quint32 id) : Medicine(id)
{
    SetState("ointment");
    burn_heal_ = 1000;
    name = "Ointment";
}

BruisePack::BruisePack(quint32 id) : Medicine(id)
{
    SetState("brutepack");
    brute_heal_ = 1000;
    name = "bruise pack";
}
