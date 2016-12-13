#include "MedbayTools.h"

HealthAnalyzer::HealthAnalyzer(quint32 id) : Item(id)
{
    SetSprite("icons/device.dmi");
    SetState("health");
}

void HealthAnalyzer::Scan(IdPtr<Human> target)
{
    int health_digit = target->GetHealth() % 10000;
    int health = (target->GetHealth() - health_digit) / 100;
    int suffocation_digit = target->GetSuffocationDamage() % 10000;
    int suffocation_damage = (target->GetSuffocationDamage() - suffocation_digit) / 100;
    int burn_digit = target->GetBurnDamage() % 10000;
    int burn = (target->GetBurnDamage() - burn_digit) / 100;
    int brute_digit = target->GetBruteDamage() % 10000;
    int brute = (target->GetBruteDamage() - brute_digit) / 100;

    GetGame().GetChat().PostTextFor(QString("Analyzing results for %1:\nOverall status:"), GetOwner());

    GetGame().GetChat().PostTextFor(target->IsDead() ? QString("Deceased") : QString("%1.%2% healthy").arg(health).arg(health_digit), GetOwner());
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
    GetGame().GetChat().PostTextFor(QString("Damage: Brute-Burn-Suffocation Specfics: %1.%2-%3.%4-%5.%6").arg(brute).arg(brute_digit).arg(burn).arg(burn_digit).arg(suffocation_damage).arg(suffocation_digit), GetOwner());
}
Medicine::Medicine(quint32 id) : Item(id)
{
    SetSprite("icons/item.dmi");
    burn_heal_ = 0;
    brute_heal_ = 0;
}

void Medicine::Heal(IdPtr<Human> target)
{
    target->AddDamage(-(brute_heal_),0,-(burn_heal_));
}

Ointment::Ointment(quint32 id) : Medicine(id)
{
    SetState("ointment");
    burn_heal_ = 10;
}

BrutePatch::BrutePatch(quint32 id) : Medicine(id)
{
    SetState("brutepack");
    brute_heal_ = 10;
}
