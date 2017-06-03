#pragma once

#include <QVector>

#include "Constheader.h"

namespace kv
{

struct Phrase
{
    // TODO:
};

class Hearer
{
public:
    virtual const QVector<PosPoint>& GetHeardPoints() const = 0;
    virtual void Hear(const Phrase& phrase) = 0;
};

}
