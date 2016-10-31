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
        const QByteArray data = value.toUtf8();
        const int size = data.size();

        Preallocate(size + 4);
        Write(size);

        void* dest = &(data_.data()[index_]);
        const void* src = data.constData();
        memcpy(dest, src, size);

        index_ += size;
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
