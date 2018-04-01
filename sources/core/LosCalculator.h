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
    bool Istransparent(kv::Position p) const;
    bool BresenX(kv::Position source, kv::Position target) const;
    bool BresenY(kv::Position source, kv::Position target) const;
    bool RayTrace(kv::Position source, kv::Position target) const;
    void MarkTilesOfCornerAsVisible(
        VisiblePoints* retlist,
        kv::Position at,
        kv::Position center,
        std::vector<char>* visibility) const;

    // TODO: Cache other helpful stuff, like visibility array
    mutable const MapInterface* map_;
};
