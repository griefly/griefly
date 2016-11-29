#include "MedbayTools.h"

HealthAnalyzer::HealthAnalyzer(quint32 id) : Item(id)
{
    SetSprite("icons/human.dmi");
    SetState("african1_m_s");
}

void HealthAnalyzer::Scan(IdPtr<Human> target)
{

}

Medicine::Medicine(quint32 id) : Item(id)
{
    SetSprite("icons/human.dmi");
    burn_heal_ = 0;
    brute_heal_ = 0;
}

void Medicine::Heal(IdPtr<Human> target)
{
    target->AddDamage(-(brute_heal_),0,-(burn_heal_));
}

Ointment::Ointment(quint32 id) : Medicine(id)
{
    SetState("african1_m_s");
    burn_heal_ = 10;
}

BrutePatch::BrutePatch(quint32 id) : Medicine(id)
{
    SetState("african1_m_s");
    brute_heal_ = 10;
}
