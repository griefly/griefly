#pragma once

#include <QVector>

#include "core_headers/KvGlobals.h"

namespace kv
{

struct Phrase
{
    QString text;
    QString from;
};

class Hearer
{
public:
    virtual QVector<Position> GetHeardPoints() const = 0;
    virtual void Hear(const Phrase& phrase) = 0;
};

}
