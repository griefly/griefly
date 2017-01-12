#include "MedbayTools.h"

HealthAnalyzer::HealthAnalyzer(quint32 id) : Item(id)
{
    SetSprite("icons/device.dmi");
    SetState("health");
    name = "Health Analyzer";
}

void HealthAnalyzer::Scan(IdPtr<Human> target)
{
    float health = target->GetHealth() / 100.0f;
    float suffocation_damage = target->GetSuffocationDamage() / 100.0f;
    float burn = target->GetBurnDamage() / 100.0f;
    float brute = target->GetBruteDamage() / 100.0f;

    QString results_for = QString("Analyzing results for %1:").arg(target->name);
    GetGame().GetChat().PostTextFor(results_for, GetOwner());
    GetGame().GetChat().PostTextFor("Overall status:", GetOwner());

    GetGame().GetChat().PostTextFor(target->IsDead() ? QString("Deceased") : QString("%1 healthy").arg(health), GetOwner());
    if (target->GetBruteDamage() > HUMAN_MAX_HEALTH / 10)
    {
        GetGame().GetChat().PostTextFor(QString("%1 tissue damage detected.").arg(target->GetBruteDamage() > HUMAN_MAX_HEALTH / 2 ? "Severe" : "Minor"), GetOwner());
    }
    if (target->GetBurnDamage() > HUMAN_MAX_HEALTH / 10)
    {
        GetGame().GetChat().PostTextFor(QString("%1 burn damage detected.").arg(target->GetBurnDamage() > HUMAN_MAX_HEALTH / 2 ? "Severe" : "Minor"), GetOwner());
    }
    if (target->GetSuffocationDamage() > HUMAN_MAX_HEALTH / 10)
    {
        GetGame().GetChat().PostTextFor(QString("%1 oxygen deprivation detected.").arg(target->GetSuffocationDamage() > HUMAN_MAX_HEALTH / 2 ? "Severe" : "Minor"), GetOwner());
    }
    GetGame().GetChat().PostTextFor(QString("Damage: Brute-Burn-Suffocation Specfics: %1-%2-%3").arg(brute).arg(burn).arg(suffocation_damage), GetOwner());
}
Medicine::Medicine(quint32 id) : Item(id)
{
    burn_heal_ = 0;
    brute_heal_ = 0;
}

void Medicine::Heal(IdPtr<Human> target)
{
    target->ApplyBurnDamage(-1 * burn_heal_);
    target->ApplyBruteDamage(-1 * brute_heal_);
    Delete();
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
