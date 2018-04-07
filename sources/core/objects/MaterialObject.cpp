#include <cassert>

#include "MaterialObject.h"
#include "Map.h"
#include "mobs/Mob.h"
#include "Tile.h"

using namespace kv;

void MaterialObject::SetSprite(const QString& sprite)
{
    GetView().SetSprite(sprite);
}

void MaterialObject::SetState(const QString& name)
{
    GetView().SetState(name);
}

void MaterialObject::Represent(GrowingFrame* frame, IdPtr<Mob> mob) const
{ 
    FrameData::Entity ent;
    ent.id = GetId();
    ent.click_id = GetId();
    ent.pos_x = GetPosition().x;
    ent.pos_y = GetPosition().y;
    ent.vlevel = GetVisibleLevel();
    ent.view = GetView().GetRawData();
    ent.dir = Dir::SOUTH;
    frame->Append(ent);
}

void MaterialObject::Delete()
{
    MapObject::Delete();
}

MaterialObject::MaterialObject()
{
    SetVisibleLevel(0);

    passable_all_ = passable::FULL;
    passable_up_ = passable::FULL;
    passable_down_ = passable::FULL;
    passable_left_ = passable::FULL;
    passable_right_ = passable::FULL;

    SetPassableLevel(passable::FULL);

    SetTransparency(true);
    SetName("nameless");
}
