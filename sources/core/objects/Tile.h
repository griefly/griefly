#pragma once

#include <vector>

#include "turfs/Turf.h"
#include "MapObject.h"
#include "MaterialObject.h"
#include "atmos/AtmosHolder.h"
#include "SaveableOperators.h"

namespace kv
{

class CubeTile : public MapObject
{
public:
    DECLARE_SAVEABLE(CubeTile, MapObject);
    REGISTER_CLASS_AS(CubeTile);
    CubeTile();
    virtual bool AddObject(IdPtr<MapObject> item) override;
    virtual bool RemoveObject(IdPtr<MapObject> item) override;
    virtual IdPtr<MapObject> GetNeighbour(Dir direct) const override;
    IdPtr<CubeTile> GetNeighbourImpl(Dir direct) const;
    virtual void ForEach(std::function<void(IdPtr<MapObject>)> callback) override;
    virtual PassableLevel GetPassable(Dir direct) const override;
    virtual bool Istransparent() const override;

    virtual bool CanTouch(IdPtr<MapObject> item) const override;
    virtual bool Contains(IdPtr<MapObject> item) const override;

    virtual void Bump(const Vector& force, IdPtr<Movable> item) override;
    virtual void BumpByGas(const Vector& force, bool inside = false) override;

    void SetPos(Position position)
    {
        position_ = position;
    }
    virtual Position GetPosition() const override
    {
        return position_;
    }
    virtual IdPtr<Turf> GetTurf() const override
    {
        return turf_;
    }
    virtual void SetTurf(IdPtr<Turf> turf) override
    {
        turf_ = turf;
        if (turf_.IsValid())
        {
            turf_->SetOwner(GetId());
        }
        UpdatePassable();
    }
    virtual void UpdatePassable() override;
    virtual atmos::AtmosHolder* GetAtmosHolder() override { return &atmos_holder_; }

    virtual void ApplyFire(int intensity) override;

    using ContentType = QVector<IdPtr<MaterialObject>>;
    ContentType& GetContent() { return content_; }
    const ContentType& GetContent() const { return content_; }

    void UpdateAtmosPassable();
protected:
    virtual quint32 GetItemImpl(int type_index) override;
private:
    bool CanTouch(IdPtr<MapObject> item, Dir dir) const;
    bool CanTouch(IdPtr<MapObject> item, Dir first_dir, Dir second_dir) const;

    void MoveToDir(Dir dir, Position* position) const;

    IdPtr<Turf> KV_SAVEABLE(turf_);

    atmos::AtmosHolder KV_SAVEABLE(atmos_holder_);

    Position KV_SAVEABLE(position_);

    ContentType KV_SAVEABLE(content_);

    PassableLevel KV_SAVEABLE(sum_passable_all_);
    PassableLevel KV_SAVEABLE(sum_passable_up_);
    PassableLevel KV_SAVEABLE(sum_passable_down_);
    PassableLevel KV_SAVEABLE(sum_passable_left_);
    PassableLevel KV_SAVEABLE(sum_passable_right_);
};
END_DECLARE(CubeTile);

}
