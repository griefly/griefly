#pragma once

#include <list>

#include "MapObject.h"
#include "representation/ViewInfo.h"

namespace kv
{

class MaterialObject : public MapObject
{
public:
    MaterialObject();
    DECLARE_SAVEABLE(MaterialObject, MapObject);
    REGISTER_CLASS_AS(ionmapitem);

    virtual PassableLevel GetPassable(Dir direct) const override
    {
        switch (direct)
        {
        case Dir::UP:
            return passable_up;
        case Dir::DOWN:
            return passable_down;
        case Dir::LEFT:
            return passable_left;
        case Dir::RIGHT:
            return passable_right;
        case Dir::ALL:
            return passable_all;
        default:
            break;
        }
        return passable::FULL;
    }

    void SetPassable(Dir direct, PassableLevel p)
    {
        switch (direct)
        {
        case Dir::UP:
            passable_up = p;
            break;
        case Dir::DOWN:
            passable_down = p;
            break;
        case Dir::LEFT:
            passable_left = p;
            break;
        case Dir::RIGHT:
            passable_right = p;
            break;
        case Dir::ALL:
            passable_all = p;
            break;
        }
        if (IdPtr<MapObject> owner = GetOwner())
        {
            owner->UpdatePassable();
        }
    }

    virtual bool IsTransparent() const override
    {
        return transparent;
    }
    
    virtual void Delete() override;
    virtual void Represent() override;
    void SetSprite(const QString& sprite);
    void SetState(const QString& name);
    ViewInfo* GetView() { return &view_; }
public:
    int KV_SAVEABLE(v_level);
    PassableLevel KV_SAVEABLE(passable_down);
    PassableLevel KV_SAVEABLE(passable_up);
    PassableLevel KV_SAVEABLE(passable_left);
    PassableLevel KV_SAVEABLE(passable_right);
    PassableLevel KV_SAVEABLE(passable_all);

    PassableLevel KV_SAVEABLE(passable_level);

    bool KV_SAVEABLE(transparent);
    QString KV_SAVEABLE(name);
protected:
    ViewInfo KV_SAVEABLE(view_);
};
END_DECLARE(MaterialObject);

}
