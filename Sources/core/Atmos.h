#pragma once

#include <array>
#include <vector>

#include "AtmosHolder.h"
#include "Constheader.h"

class SyncRandom;
class MapMaster;

class Atmosphere
{
public:
    Atmosphere(SyncRandom* random, MapMaster* map);
    void Resize(size_t x, size_t y, size_t z);
    void Process();
    void ProcessMove();
private:
    void ProcessTile(size_t x, size_t y, size_t z); 
    void ProcessTileMove(size_t x, size_t y, size_t z);

    void ShuffleX();
    void ShuffleY();
    void ShuffleDir();
    std::vector<size_t> x_shuffle_;
    std::vector<size_t> y_shuffle_;
    size_t z_size_;

    std::array<Dir, 4> dir_shuffle_;

    SyncRandom* random_;
    MapMaster* map_;
};
