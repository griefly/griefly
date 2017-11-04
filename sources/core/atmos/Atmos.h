#pragma once

#include <array>
#include <vector>

#include "core/KvGlobals.h"

#include "AtmosHolder.h"
#include "Interfaces.h"

class MapInterface;
namespace atmos
{
    class AtmosGrid;
}

class Atmosphere : public AtmosInterface
{
public:
    Atmosphere();

    virtual void Process(qint32 game_tick) override;
    virtual void ProcessMove(qint32 game_tick) override;

    virtual void Represent(kv::GrowingFrame* frame) const;

    virtual void SetFlags(quint32 x, quint32 y, quint32 z, Flags flags) override;
    virtual void LoadGrid(MapInterface* map) override;
private:
    void AssertGrid();

    void Resize(quint32 x, quint32 y, quint32 z);

    atmos::AtmosGrid* grid_;

    qint64 grid_processing_ns_;
    qint64 movement_processing_ns_;

    void ProcessTileMove(int x, int y, int z, qint32 game_tick);

    int x_size_;
    int y_size_;
    int z_size_;

    MapInterface* map_;
};
