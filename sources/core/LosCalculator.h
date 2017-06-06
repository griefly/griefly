#pragma once

#include "Interfaces.h"

#include <QVector>

class LosCalculator
{
public:
    LosCalculator();
    void Calculate(const MapInterface* map, VisiblePoints* retval, int posx, int posy, int posz = 0) const;
private:
    static int PosToCorner(int pos);
    static int CornerToPos(int corner);
    static int Sign(int value);
    static kv::Position CornerPointToPoint(kv::Position p);

    bool CheckCorner(kv::Position p) const;
    bool CheckBorders(int x, int y, int z) const;
    bool IsTransparent(kv::Position p) const;
    bool BresenX(kv::Position source, kv::Position target) const;
    bool BresenY(kv::Position source, kv::Position target) const;
    bool RayTrace(kv::Position source, kv::Position target) const;
    void MarkTilesOfCornerAsVisible(
        VisiblePoints* retlist,
        kv::Position at,
        kv::Position center,
        std::vector<char>* visibility) const;

    // TODO: Make this class as pure function, because the only member class is not needed
    mutable const MapInterface* map_;
};
