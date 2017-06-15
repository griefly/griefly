#pragma once

#include "Item.h"

namespace kv
{

class Cloth : public Item
{
public:
    DECLARE_SAVEABLE(Cloth, Item);
    REGISTER_CLASS_AS(Cloth);
    Cloth();
};
END_DECLARE(Cloth);

class Armor : public Cloth
{
public:
    DECLARE_SAVEABLE(Armor, Cloth);
    REGISTER_CLASS_AS(Armor);
    Armor();
};
END_DECLARE(Armor);

class Helmet : public Cloth
{
public:
    DECLARE_SAVEABLE(Helmet, Cloth);
    REGISTER_CLASS_AS(Helmet);
    Helmet();
};
END_DECLARE(Helmet);

class Tophat : public Cloth
{
public:
    DECLARE_SAVEABLE(Tophat, Cloth);
    REGISTER_CLASS_AS(Tophat);
    Tophat();
};
END_DECLARE(Tophat);

class EngineUniform : public Cloth
{
public:
    DECLARE_SAVEABLE(EngineUniform, Cloth);
    REGISTER_CLASS_AS(EngineUniform);
    EngineUniform();
};
END_DECLARE(EngineUniform);

class JanitorUniform : public Cloth
{
public:
    DECLARE_SAVEABLE(JanitorUniform, Cloth);
    REGISTER_CLASS_AS(JanitorUniform);
    JanitorUniform();
};
END_DECLARE(JanitorUniform);

class ClownUniform : public Cloth
{
public:
    DECLARE_SAVEABLE(ClownUniform, Cloth);
    REGISTER_CLASS_AS(ClownUniform);
    ClownUniform();
};
END_DECLARE(ClownUniform);

class BarmanUniform : public Cloth
{
public:
    DECLARE_SAVEABLE(BarmanUniform, Cloth);
    REGISTER_CLASS_AS(BarmanUniform);
    BarmanUniform();
};
END_DECLARE(BarmanUniform);

class RedUniform : public Cloth
{
public:
    DECLARE_SAVEABLE(RedUniform, Cloth);
    REGISTER_CLASS_AS(RedUniform);
    RedUniform();
};
END_DECLARE(RedUniform);

class OrangeBoots : public Cloth
{
public:
    DECLARE_SAVEABLE(OrangeBoots, Cloth);
    REGISTER_CLASS_AS(OrangeBoots);
    OrangeBoots();
};
END_DECLARE(OrangeBoots);

class BlackBoots : public Cloth
{
public:
    DECLARE_SAVEABLE(BlackBoots, Cloth);
    REGISTER_CLASS_AS(BlackBoots);
    BlackBoots();
};
END_DECLARE(BlackBoots);

class ClownBoots : public Cloth
{
public:
    DECLARE_SAVEABLE(ClownBoots, Cloth);
    REGISTER_CLASS_AS(ClownBoots);
    ClownBoots();
};
END_DECLARE(ClownBoots);

class MedicalUniform : public Cloth
{
public:
    DECLARE_SAVEABLE(MedicalUniform, Cloth);
    REGISTER_CLASS_AS(MedicalUniform);
    MedicalUniform();
};
END_DECLARE(MedicalUniform);

class WhiteShoes : public Cloth
{
public:
    DECLARE_SAVEABLE(WhiteShoes, Cloth);
    REGISTER_CLASS_AS(WhiteShoes);
    WhiteShoes();
};
END_DECLARE(WhiteShoes);

}
