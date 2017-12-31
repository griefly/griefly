#include "Weapons.h"
#include "objects/Tile.h"

using namespace kv;

LaserGun::LaserGun()
{
    SetState("energykill100");

    name = "Laser Gun";
    
    ammunition_ = 20;
    max_ammunition_ = 20;
}

void LaserGun::Shoot(Vector target)
{
    unsigned int value = GenerateRandom() % 2;
    QString snd;
    if (value == 0)
    {
        snd = "laser3.wav";
    }
    if (value == 1)
    {
        snd = "Laser.wav";
    }
    ShootImpl(target, snd, Laser::GetTypeStatic(), "");
}
Revolver::Revolver()
{
    SetState("revolver");

    name = "Revolver";
    
    ammunition_ = 6;
    max_ammunition_ = 6;
}

void Revolver::Shoot(Vector target)
{
    QString snd;
    snd = "Gunshot.wav";
    ShootImpl(target, snd, Bullet::GetTypeStatic(), BulletCasing::GetTypeStatic());
}
