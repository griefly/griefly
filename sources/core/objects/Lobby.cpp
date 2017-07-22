#include "Lobby.h"

using namespace kv;

Lobby::Lobby()
{
    seconds_ = 1;//60 * 2;
}

void Lobby::AfterWorldCreation()
{
    Object::AfterWorldCreation();
    SetFreq(10);
}

int Lobby::GetSecondUntilStart()
{
    return seconds_;
}

void Lobby::Process()
{
    --seconds_;
}

void Lobby::AddSpawnPoint(IdPtr<SpawnPoint> point)
{
    spawn_points_.push_back(point);
}

QVector<IdPtr<CubeTile>> Lobby::GetTilesFor(const QString& type)
{
    QVector<IdPtr<CubeTile>> retval;
    for (auto it = spawn_points_.begin(); it != spawn_points_.end(); ++it)
    {
        if ((*it)->spawn_type == type)
        {
            retval.append((*it)->GetOwner());
        }
    }
    return retval;
}
