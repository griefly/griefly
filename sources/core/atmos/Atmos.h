#pragma once

#include <array>
#include <vector>

#include "AtmosHolder.h"
#include "../Constheader.h"
#include "Interfaces.h"

class IMap;
namespace atmos
{
    class AtmosGrid;
}
class TextPainter;

class Atmosphere : public IAtmosphere
{
public:
    Atmosphere(IMap* map, TextPainter* texts);

    virtual void Process() override;
    virtual void ProcessMove() override;

    virtual void SetFlags(quint32 x, quint32 y, quint32 z, Flags flags) override;
    // TODO: LoadGrid(IMap* map), so IAtmosphere could be injected into Map
    virtual void LoadGrid() override;
private:
    void Resize(quint32 x, quint32 y, quint32 z);

    atmos::AtmosGrid* grid_;

    qint64 grid_processing_ns_;
    qint64 movement_processing_ns_;

    void ProcessTileMove(int x, int y, int z);

    int x_size_;
    int y_size_;
    int z_size_;

    IMap* map_;
    TextPainter* texts_;
};
