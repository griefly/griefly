#pragma once

#include <qglobal.h>

#include <vector>

class FastSerializer
{
public:
    FastSerializer();
    ~FastSerializer();

    void ResetIndex()
    {
        index_ = 0;
    }
    quint32 GetIndex()
    {
        return index_;
    }
    char* GetData()
    {
        return data_.data();
    }

    void Write(bool value)
    {
        Preallocate(1);
        data_[index_] = value;
        ++index_;
    }

private:
    void Preallocate(int size)
    {
        if ((index_ + size) > data_.size())
        {
            data_.resize(data_.size() * 2);
        }
    }

    std::vector<char> data_;
    quint32 index_;
};
