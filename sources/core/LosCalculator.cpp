#include "LosCalculator.h"

const int RAY_MULTIPLIER = 2;

int LosCalculator::PosToCorner(int pos)
{
    return pos * RAY_MULTIPLIER;
}

int LosCalculator::CornerToPos(int corner)
{
    return corner / RAY_MULTIPLIER;
}

int LosCalculator::Sign(int value)
{
    if (value > 0)
    {
        return 1;
    }
    else if (value < 0)
    {
        return -1;
    }
    return 0;
}

bool LosCalculator::CheckCorner(PosPoint p) const
{
    int x = CornerToPos(p.posx);
    int y = CornerToPos(p.posy);
    int z = CornerToPos(p.posz);
    return CheckBorders(x, y, z);
}

bool LosCalculator::CheckBorders(const int x, const int y, const int z) const
{
    if (   x < 0
        || x >= map_->GetWidth())
    {
        return false;
    }
    if (   y < 0
        || y >= map_->GetHeight())
    {
        return false;
    }
    if (   z < 0
        || z >= map_->GetDepth())
    {
        return false;
    }
    return true;
}


PosPoint LosCalculator::CornerPointToPoint(PosPoint p)
{
    PosPoint retval(CornerToPos(p.posx), CornerToPos(p.posy), CornerToPos(p.posz));
    return retval;
}

bool LosCalculator::IsTransparent(PosPoint p) const
{
    PosPoint tilePoint = CornerPointToPoint(p);
    return map_->IsTransparent(tilePoint.posx, tilePoint.posy, tilePoint.posz);
}

bool LosCalculator::BresenX(PosPoint source, PosPoint target) const
{
    int y = source.posy;
    int error = 0;
    int delta_x = std::abs(source.posx - target.posx);
    int deltaerr = std::abs(source.posy - target.posy);
    int deltastep = Sign(target.posy - source.posy);
    int incrstep = Sign(target.posx - source.posx);
    for (int x = source.posx; x != target.posx; x += incrstep)
    {
        if ((x % RAY_MULTIPLIER) == 0 && (y % RAY_MULTIPLIER) == 0)
        {
            // when in corner check side neighbours
            // if both of them are not transparent then corner is not transparent
            PosPoint left_neighbour(x + incrstep, y, source.posz);
            PosPoint right_neighbour(x, y + deltastep, source.posz);
            if (!IsTransparent(left_neighbour) && !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else if (x % RAY_MULTIPLIER == 0)
        {
            // when ray hits an edge check both tiles - current and previous. Since ray travels through edge both of them
            // must be transparent
            PosPoint left_neighbour(x - incrstep, y, source.posz);
            PosPoint right_neighbour(x + incrstep, y, source.posz);
            if (!IsTransparent(left_neighbour) || !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else if (y % RAY_MULTIPLIER == 0)
        {
            // second case of edge handling
            PosPoint left_neighbour(x, y - deltastep, source.posz);
            PosPoint right_neighbour(x, y + deltastep, source.posz);
            if (!IsTransparent(left_neighbour) || !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else
        {
            PosPoint new_point(x, y, source.posz);
            if (!IsTransparent(new_point))
            {
                return false;
            }
        }

        error += deltaerr;
        if (error >= delta_x)
        {
            y += deltastep;
            error -= delta_x;
        }
    }

    return true;
}

bool LosCalculator::BresenY(PosPoint source, PosPoint target) const
{
    int x = source.posx;
    int error = 0;
    int delta_y = std::abs(source.posy - target.posy);
    int deltaerr = std::abs(source.posx - target.posx);
    int deltastep = Sign(target.posx - source.posx);
    int incrstep = Sign(target.posy - source.posy);
    for (int y = source.posy; y != target.posy; y += incrstep)
    {
        if ((x % RAY_MULTIPLIER) == 0 && (y % RAY_MULTIPLIER) == 0)
        {
            // when in corner check side neighbours
            // if both of them are not transparent then corner is not transparent
            PosPoint left_neighbour(x + deltastep, y, source.posz);
            PosPoint right_neighbour(x, y + incrstep, source.posz);
            if (!IsTransparent(left_neighbour) && !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else if (x % RAY_MULTIPLIER == 0)
        {
            // when ray hits an edge check both tiles. Since ray travels through edge both of them
            // must be transparent
            PosPoint left_neighbour(x - deltastep, y, source.posz);
            PosPoint right_neighbour(x + deltastep, y, source.posz);
            if (!IsTransparent(left_neighbour) || !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else if (y % RAY_MULTIPLIER == 0)
        {
            // second case of edge handling
            PosPoint left_neighbour(x, y - incrstep, source.posz);
            PosPoint right_neighbour(x, y + incrstep, source.posz);
            if (!IsTransparent(left_neighbour) || !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else
        {
            PosPoint new_point(x, y, source.posz);

            if (!IsTransparent(new_point))
            {
                return false;
            }
        }

        error += deltaerr;
        if (error >= delta_y)
        {
            x += deltastep;
            error -= delta_y;
        }
    }

    return true;
}

bool LosCalculator::RayTrace(PosPoint source, PosPoint target) const
{
    // run Bresenham's line algorithm
    if (std::abs(source.posx - target.posx) > std::abs(source.posy - target.posy))
    {
        return BresenX(source, target);
    }
    else
    {
        return BresenY(source, target);
    }

    return false;
}

void LosCalculator::MarkTilesOfCornerAsVisible(
    VisiblePoints* retlist,
    PosPoint at,
    PosPoint center,
    std::vector<char>* visibility) const
{
    for (int dx = -1; dx <= 0; dx++)
    {
        for (int dy = -1; dy <= 0; dy++)
        {
            PosPoint p(at.posx + dx, at.posy + dy, at.posz);
            if (!CheckBorders(p.posx, p.posy, p.posz))
            {
                continue;
            }

            int vis_x = (p.posx - center.posx + SIZE_W_SQ);
            int vis_y = (p.posy - center.posy + SIZE_H_SQ);
            int vis_idx = 2 * SIZE_H_SQ * vis_x + vis_y;

            if (vis_idx < 0)
            {
                continue;
            }

            if ((*visibility)[vis_idx] == 1)
            {
                continue;
            }

            retlist->push_back(p);
            (*visibility)[vis_idx] = 1;
        }
    }
}

LosCalculator::LosCalculator()
{
    map_ = nullptr;
}

// LOSfinder::calculateVisisble calculates visibility list of map from given map point
// The line-of-sigh check works as follows:
// from the center of given tile to every tile corner of the viewport ray is casted
// ray is casted from center of tile to corners, so it is never aligned to either axis
// if ray passes only transparent tiles on its way to given corner then the corner is visible
// if ray passes through corner then it checks sibling tiles. If both of them are not transparent then ray blocks
// if ray passes through edge it checks both adjasent tiles. They both must be transparent, otherwise ray blocks
// if tile has at least one visible corner then this tile is visible
// otherwise tile is invisible
void LosCalculator::Calculate(const MapInterface* map, VisiblePoints* retlist, int posx, int posy, int posz) const
{
    map_ = map;

    const int VISIBLE_TILES_SIZE = 4 * (SIZE_H_SQ + 2) * (SIZE_W_SQ + 2);

    std::vector<char> visible_tiles;
    visible_tiles.resize(VISIBLE_TILES_SIZE);

    for (int i = 0; i < VISIBLE_TILES_SIZE; ++i)
    {
        visible_tiles[i] = 0;
    }

    PosPoint source(
          PosToCorner(posx) + RAY_MULTIPLIER / 2,
          PosToCorner(posy) + RAY_MULTIPLIER / 2,
          PosToCorner(posz) + 1);
    for (int i = -SIZE_W_SQ; i < SIZE_W_SQ; ++i)
    {
        for (int j = -SIZE_H_SQ; j < SIZE_H_SQ; ++j)
        {
            PosPoint p(PosToCorner(posx + i), PosToCorner(posy + j), PosToCorner(posz));
            if (!CheckCorner(p))
            {
                continue;
            }

            // TODO: we can check that all siblings of this corner are visible
            // so check is unnessesary

            if (RayTrace(source, p))
            {
                // add all tiles with this corner to visible list
                MarkTilesOfCornerAsVisible(
                    retlist, CornerPointToPoint(p), CornerPointToPoint(source), &visible_tiles);
            }
        }
    }
}

