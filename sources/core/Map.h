#pragma once

#include <cstddef>

#include <list>
#include <vector>

#include "Constheader.h"
#include "objects/Tile.h"
#include "atmos/Atmos.h"

#include "Interfaces.h"

class LosCalculator
{
public:
    LosCalculator(IMap* map);
    void Calculate(std::list<PosPoint>* retval, int posx, int posy, int posz = 0);
private:
    static int PosToCorner(int pos);
    static int CornerToPos(int corner);
    static int Sign(int value);
    static PosPoint CornerPointToPoint(PosPoint p);

    bool CheckCorner(PosPoint p);
    bool CheckBorders(int x, int y, int z);
    bool IsTransparent(PosPoint p);
    bool BresenX(PosPoint source, PosPoint target);
    bool BresenY(PosPoint source, PosPoint target);
    bool RayTrace(PosPoint source, PosPoint target);
    void MarkTilesOfCornerAsVisible(
            std::list<PosPoint>* retlist,
            PosPoint at,
            PosPoint center,
            char visibility[]);
    void Clear();
    std::list<PosPoint> worklist_;

    IMap* map_;
};

class Map : public IMap
{
public:
    Map();
    ~Map();

    typedef IdPtr<CubeTile> SqType;

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

    virtual void CalculateVisisble(std::list<PosPoint>* retval, int posx, int posy, int posz = 0) override;
private:
    LosCalculator los_calculator_;
    IAtmosphere* atmosphere_;
    QVector<QVector<QVector<SqType>>> squares_;
    std::list<PosPoint> visible_points_;
};
