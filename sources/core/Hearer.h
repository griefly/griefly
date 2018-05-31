#pragma once

#include <QVector>

#include "KvGlobals.h"

namespace kv
{

struct Phrase
{
    QString text;
    QString from;
    QString expression;
};

class Hearer
{
public:
    virtual QVector<Position> GetHeardPoints() const = 0;
    virtual void Hear(const Phrase& phrase) = 0;
};

}
