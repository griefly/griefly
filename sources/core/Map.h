#pragma once

#include <cstddef>

#include <list>
#include <vector>

#include "Constheader.h"
#include "objects/Tile.h"
#include "atmos/Atmos.h"

#include "Interfaces.h"

#include "LosCalculator.h"

class Map : public MapInterface
{
public:
    Map();
    ~Map();

    virtual SqType& At(int x, int y, int z) override;
    virtual const SqType& At(int x, int y, int z) const override;

    virtual int GetWidth() const override;
    virtual int GetHeight() const override;
    virtual int GetDepth() const override;

    virtual std::list<PosPoint>* GetVisiblePoints() override;

    virtual void Resize(int new_x, int new_y, int new_z) override;
    virtual void FillTilesAtmosHolders() override;

    virtual void Represent() override;

    virtual bool IsTileVisible(quint32 tile_id) override;
    virtual bool IsTransparent(int posx, int posy, int posz = 0) override;

    virtual void CalculateLos(std::list<PosPoint>* retval, int posx, int posy, int posz = 0) override;
private:
    LosCalculator los_calculator_;
    AtmosInterface* atmosphere_;
    QVector<QVector<QVector<SqType>>> squares_;
    std::list<PosPoint> visible_points_;
};
