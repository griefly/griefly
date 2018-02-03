#include "ObjectProcessor.h"

using namespace kv;

ObjectProcessor::ObjectProcessor()
{
    // Nothing
}

void ObjectProcessor::Add(quint32 object)
{
    const auto it = objects_places_.find(object);
    if (it != objects_places_.end())
    {
        return;
    }

    objects_places_[object] = objects_.size();
    objects_.append(object);
}

void ObjectProcessor::RunProcess()
{
    const qint32 current_game_tick = GetGameTick();
    for (auto& object : objects_)
    {
        const int object_freq = object->GetFreq();

        if (object_freq == 0)
        {
            // TODO: remove object from list
            return;
        }

        if ((current_game_tick % object_freq) == 0)
        {
            object->Process();
        }
    }
}

void ObjectProcessor::LoadObjectsPlaces()
{
    for (int i = 0; i < objects_.size(); ++i)
    {
        objects_places_[objects_[i].Id()] = i;
    }
}
