#pragma once

#include "KvGlobals.h"
#include "objects/Tile.h"
#include "objects/Object.h"
#include "atmos/Atmos.h"

#include "Interfaces.h"

#include "LosCalculator.h"

#include "SaveableOperators.h"

namespace kv
{

class Map : public Object, public MapInterface
{
public:
    DECLARE_SAVEABLE(Map, Object);
    REGISTER_CLASS_AS(Map);

    Map();
    ~Map();

    virtual SqType& At(int x, int y, int z) override;
    virtual const SqType& At(int x, int y, int z) const override;

    virtual int GetWidth() const override;
    virtual int GetHeight() const override;
    virtual int GetDepth() const override;

    virtual void Resize(int new_x, int new_y, int new_z) override;
    virtual void FillTilesAtmosHolders() override;

    virtual void Represent(GrowingFrame* frame, const VisiblePoints& points, IdPtr<Mob> mob) const override;

    virtual bool Istransparent(int posx, int posy, int posz = 0) const override;

    virtual void CalculateLos(VisiblePoints* retval, int posx, int posy, int posz = 0) const override;
private:
    LosCalculator los_calculator_;
    QVector<QVector<QVector<SqType>>> KV_SAVEABLE(squares_);
};
END_DECLARE(Map)

}
