#pragma once

#include <list>

#include "MapObject.h"
#include "ViewInfo.h"

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
        case Dir::NORTH:
            return passable_up_;
        case Dir::SOUTH:
            return passable_down_;
        case Dir::WEST:
            return passable_left_;
        case Dir::EAST:
            return passable_right_;
        case Dir::ALL:
            return passable_all_;
        default:
            break;
        }
        return passable::FULL;
    }

    void SetPassable(Dir direct, PassableLevel p)
    {
        switch (direct)
        {
        case Dir::NORTH:
            passable_up_ = p;
            break;
        case Dir::SOUTH:
            passable_down_ = p;
            break;
        case Dir::WEST:
            passable_left_ = p;
            break;
        case Dir::EAST:
            passable_right_ = p;
            break;
        case Dir::ALL:
            passable_all_ = p;
            break;
        default:
            break;
        }
        if (IdPtr<MapObject> owner = GetOwner())
        {
            owner->UpdatePassable();
        }
    }

    void SetTransparency(bool transparent) { transparent_ = transparent; }
    virtual bool Istransparent() const override
    {
        return transparent_;
    }
    
    virtual void Delete() override;
    virtual void Represent(GrowingFrame* frame, IdPtr<kv::Mob> mob) override;
    void SetSprite(const QString& sprite);
    void SetState(const QString& name);
    ViewInfo& GetView() { return view_; }
public:
    int KV_SAVEABLE(v_level);

    PassableLevel GetPassableLevel() const { return passable_level_; }
    void SetPassableLevel(PassableLevel passable_level) { passable_level_ = passable_level; }

    const QString& GetName() const { return name_; }
    void SetName(const QString& name) { name_ = name; }
private:
    PassableLevel KV_SAVEABLE(passable_down_);
    PassableLevel KV_SAVEABLE(passable_up_);
    PassableLevel KV_SAVEABLE(passable_left_);
    PassableLevel KV_SAVEABLE(passable_right_);
    PassableLevel KV_SAVEABLE(passable_all_);

    PassableLevel KV_SAVEABLE(passable_level_);

    ViewInfo KV_SAVEABLE(view_);
    QString KV_SAVEABLE(name_);
    bool KV_SAVEABLE(transparent_);
};
END_DECLARE(MaterialObject);

}
