#pragma once

#include <qglobal.h>

#include <QString>

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
    void Write(qint32 value)
    {
        Preallocate(4);

        data_[index_ + 0] = (value >>  0) & 0xFF;
        data_[index_ + 1] = (value >>  8) & 0xFF;
        data_[index_ + 2] = (value >> 16) & 0xFF;
        data_[index_ + 3] = (value >> 24) & 0xFF;

        index_ += 4;
    }
    void Write(quint32 value)
    {
        Preallocate(4);

        data_[index_ + 0] = (value >>  0) & 0xFF;
        data_[index_ + 1] = (value >>  8) & 0xFF;
        data_[index_ + 2] = (value >> 16) & 0xFF;
        data_[index_ + 3] = (value >> 24) & 0xFF;

        index_ += 4;
    }

    void Write(const QString& value)
    {
        const QChar* data = value.data();
        const int size = value.size();

        Write(size);
        Preallocate(size * 2);

        for (int i = 0; i < size; ++i)
        {
            quint16 value = data[i].unicode();
            data_[index_ + i * 2 + 0] = (value >> 0) & 0xFF;
            data_[index_ + i * 2 + 1] = (value >> 8) & 0xFF;
        }

        index_ += size * 2;
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
