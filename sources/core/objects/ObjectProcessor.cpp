#include "ObjectProcessor.h"

using namespace kv;

ObjectProcessor::ObjectProcessor()
{
    // Nothing
}

void ObjectProcessor::Add(quint32 object)
{
    const auto it = objects_lookup_.find(object);
    if (it != objects_lookup_.end())
    {
        return;
    }

    objects_lookup_.insert(object);
    objects_.append(object);
}

void ObjectProcessor::RunProcess()
{
    const qint32 current_game_tick = GetGameTick();

    int end = objects_.size();
    for (int i = 0; i < end; ++i)
    {
        auto object = objects_[i];

        if (   !object.IsValid()
            || (object->GetFreq() == 0))
        {
            objects_lookup_.erase(object.Id());
            objects_[i] = objects_.last();
            objects_.pop_back();
            --end;
            --i;
            return;
        }

        if ((current_game_tick % object->GetFreq()) == 0)
        {
            object->Process();
        }
    }
}

void ObjectProcessor::LoadObjectsPlaces()
{
    for (auto& object : objects_)
    {
        objects_lookup_.insert(object.Id());
    }
}
