#include "ObjectProcessor.h"

using namespace kv;

ObjectProcessor::ObjectProcessor()
{
    // Nothing
}

void ObjectProcessor::Add(IdPtr<Object> object)
{
    // TODO: better way to check it
    if (objects_.contains(object))
    {
        return;
    }
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
