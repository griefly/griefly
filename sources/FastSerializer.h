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
        data_[index_] = value;
        ++index_;

        if (index_ == data_.size())
        {
            data_.resize(data_.size() * 2);
        }
    }

private:
    std::vector<char> data_;
    quint32 index_;
};
