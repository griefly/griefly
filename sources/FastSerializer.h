#pragma once

#include <qglobal.h>

class FastSerializer
{
public:
    FastSerializer();
    ~FastSerializer();

    void ResetIndex()
    {
        index_ = 0;
    }
    quint32 GetSize()
    {
        return size_;
    }
    char* GetData()
    {
        return data_;
    }

    void Write(bool value)
    {
        data_[index_] = value;
        ++index_;
    }

private:
    char* data_;
    quint32 size_;
    quint32 index_;
};
