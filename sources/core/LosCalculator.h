#pragma once

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
    void MarkTilesOfCornerAsVisible(std::list<PosPoint>* retlist,
            PosPoint at,
            PosPoint center,
            std::vector<char>* visibility);
    IMap* map_;
};
