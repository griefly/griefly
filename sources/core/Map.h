#pragma once

#include "Constheader.h"
#include "objects/Tile.h"
#include "objects/Object.h"
#include "atmos/Atmos.h"

#include "Interfaces.h"

#include "LosCalculator.h"

template<class TValue>
inline FastSerializer& operator<<(FastSerializer& file, const QVector<TValue>& vector)
{
    file << vector.size();
    for (const auto& value : vector)
    {
        file << value;
    }
    return file;
}

template<class TValue>
inline FastDeserializer& operator>>(FastDeserializer& file, QVector<TValue>& vector)
{
    int size;
    file >> size;
    vector.resize(size);
    for (int i = 0; i < size; ++i)
    {
        file >> vector[i];
    }
    return file;
}

template<class TValue>
inline unsigned int hash(const QVector<TValue>& vector)
{
    unsigned int retval = 0;
    for (int i = 0; i < vector.size(); ++i)
    {
        retval += i * hash(vector[i]);
    }
    return retval;
}

namespace kv
{

class Map : public Object, public MapInterface
{
public:
    DECLARE_SAVEABLE(Map, Object);
    REGISTER_CLASS_AS(Map);

    Map();
    ~Map();

    virtual SqType& At(int x, int y, int z) override;
    virtual const SqType& At(int x, int y, int z) const override;

    virtual int GetWidth() const override;
    virtual int GetHeight() const override;
    virtual int GetDepth() const override;

    virtual void Resize(int new_x, int new_y, int new_z) override;
    virtual void FillTilesAtmosHolders() override;

    virtual void Represent(const VisiblePoints& points) const override;

    virtual bool IsTileVisible(quint32 tile_id) override;
    virtual bool IsTransparent(int posx, int posy, int posz = 0) const override;

    virtual void CalculateLos(VisiblePoints* retval, int posx, int posy, int posz = 0) const override;
private:
    LosCalculator los_calculator_;
    QVector<QVector<QVector<SqType>>> KV_SAVEABLE(squares_);
};
END_DECLARE(Map)

}
