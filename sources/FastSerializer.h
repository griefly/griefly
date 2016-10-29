#pragma once

#include <qglobal.h>

class FastSerializer
{
public:
    FastSerializer();
private:
    char* data_;
    quint32 size_;
    quint32 index_;
};

