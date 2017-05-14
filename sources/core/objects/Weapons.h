#include "Gun.h"

namespace kv
{

class LaserGun : public Gun
{
public:
    DECLARE_SAVEABLE(LaserGun, Gun);
    REGISTER_CLASS_AS(LaserGun);
    LaserGun();
    virtual void Shoot(VDir target) override;
};
END_DECLARE(LaserGun);

class Revolver : public Gun
{
public:
    DECLARE_SAVEABLE(Revolver, Gun);
    REGISTER_CLASS_AS(Revolver);
    Revolver();
    virtual void Shoot(VDir target) override;
};
END_DECLARE(Revolver);

}
