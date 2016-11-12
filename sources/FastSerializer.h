#pragma once

#include <QDebug>
#include <QString>

#include <vector>

#include "KVAbort.h"\

class FastSerializer
{
    template<class T>
    friend FastSerializer& operator<<(
        FastSerializer& serializer,
        const T& value);
public:
    static const int DEFAULT_SIZE = 32 * 1024 * 1024;
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
private:
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
    // Removed
    void Write(const char* value);
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

template<class T>
inline FastSerializer& operator<<(FastSerializer& serializer, const T& value)
{
    static_assert(
        !std::is_array<T>::value,
        "Arrays are forbidden (string constants too)!");
    serializer.Write(value);
    return serializer;
}

class FastDeserializer
{
    template<class T>
    friend FastDeserializer& operator>>(
        FastDeserializer& serializer,
        T& value);
public:
    FastDeserializer(const char* data, quint32 size)
        : data_(data),
          size_(size),
          index_(0)
    {
        // Nothing
    }
    bool IsEnd()
    {
        if (index_ >= size_)
        {
            return true;
        }
        return false;
    }

private:
    void Read(bool* value)
    {
        EnsureSize(1);

        *value = data_[index_];
        ++index_;
    }

    void Read(qint32* value)
    {
        EnsureSize(4);

        *value = 0;

        quint32 helper;

        helper = static_cast<unsigned char>(data_[index_ + 0]);
        *value |= helper << 0;

        helper = static_cast<unsigned char>(data_[index_ + 1]);
        *value |= helper << 8;

        helper = static_cast<unsigned char>(data_[index_ + 2]);
        *value |= helper << 16;

        helper = static_cast<unsigned char>(data_[index_ + 3]);
        *value |= helper << 24;

        index_ += 4;
    }

    void Read(quint32* value)
    {
        EnsureSize(4);

        *value = 0;

        quint32 helper;

        helper = static_cast<unsigned char>(data_[index_ + 0]);
        *value |= helper << 0;

        helper = static_cast<unsigned char>(data_[index_ + 1]);
        *value |= helper << 8;

        helper = static_cast<unsigned char>(data_[index_ + 2]);
        *value |= helper << 16;

        helper = static_cast<unsigned char>(data_[index_ + 3]);
        *value |= helper << 24;

        index_ += 4;
    }

    void Read(QString* value)
    {
        int size;
        Read(&size);

        EnsureSize(size * 2);
        value->resize(size);
        QChar* data = value->data();

        for (int i = 0; i < size; ++i)
        {
            quint16 unicode = 0;
            quint16 helper;
            helper = static_cast<unsigned char>(data_[index_ + i * 2 + 0]);
            unicode |= helper << 0;

            helper = static_cast<unsigned char>(data_[index_ + i * 2 + 1]);
            unicode |= helper << 8;

            data[i] = unicode;
        }

        index_ += size * 2;
    }

    void EnsureSize(int size)
    {
        if ((size + index_) > size_)
        {
            qDebug() << "FastDeserializer: EnsureSize fail!";
            KvAbort();
        }
    }

    const char* const data_;
    const quint32 size_;
    quint32 index_;
};

template<class T>
inline FastDeserializer& operator>>(FastDeserializer& deserializer, T& value)
{
    deserializer.Read(&value);
    return deserializer;
}












