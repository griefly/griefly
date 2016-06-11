#pragma once

#include <array>
#include <vector>

#include "AtmosHolder.h"
#include "Constheader.h"
#include "Interfaces.h"

class SyncRandom;
class IMapMaster;

class Atmosphere : public IAtmosphere
{
public:
    Atmosphere(SyncRandom* random, IMapMaster* map);
    virtual void Resize(size_t x, size_t y, size_t z) override;
    virtual void Process() override;
    virtual void ProcessMove() override;
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
    IMapMaster* map_;
};
