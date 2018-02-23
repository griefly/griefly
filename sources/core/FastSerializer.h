#pragma once

#include <QDebug>
#include <QString>

#include <vector>

#include "KvAbort.h"

static_assert(
    sizeof(int) == 4,
    "Unsupported platform, sizeof(int) should be 4!");
static_assert(
    sizeof(unsigned int) == 4,
    "Unsupported platform, sizeof(unsigned int) should be 4!");
static_assert(
    sizeof(ushort) == 2,
    "Unsupported platform, sizeof(ushort) should be 2!");
static_assert(
    sizeof(short) == 2,
    "Unsupported platform, sizeof(short) should be 2!");
static_assert(
    sizeof(char) == 1,
    "Unsupported platform, sizeof(char) should be 1!");
static_assert(
    sizeof(bool) == 1,
    "Unsupported platform, sizeof(bool) should be 1!");

namespace kv
{

const QLatin1String END_TYPE("0~$");

class FastSerializer
{
    template<class T>
    friend FastSerializer& operator<<(
        FastSerializer& serializer,
        const T& value);
public:
    using Type = char;

    static const Type BOOL_TYPE = 1;
    static const Type INT32_TYPE = 2;
    static const Type UINT32_TYPE = 3;
    static const Type STRING_TYPE = 4;
    static const Type BYTEARRAY_TYPE = 5;
    static const Type TYPE_TYPE = 6;
    static const Type INT64_TYPE = 7;

    static const int DEFAULT_SIZE = 32 * 1024 * 1024;
    FastSerializer(int size = DEFAULT_SIZE)
        : index_(0)
    {
        data_.resize(size);
    }

    ~FastSerializer()
    {
        // Nothing
    }

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
    void WriteType(const QString& type)
    {
        Write(type, TYPE_TYPE);
    }
private:
    void Write(bool value)
    {
        Preallocate(2);
        data_[index_] = BOOL_TYPE;
        data_[index_ + 1] = value;
        index_ += 2;
    }
    void Write(qint32 value)
    {
        Preallocate(5);

        data_[index_ + 0] = INT32_TYPE;
        data_[index_ + 1] = (value >>  0) & 0xFF;
        data_[index_ + 2] = (value >>  8) & 0xFF;
        data_[index_ + 3] = (value >> 16) & 0xFF;
        data_[index_ + 4] = (value >> 24) & 0xFF;

        index_ += 5;
    }
    void Write(quint32 value)
    {
        Preallocate(5);

        data_[index_ + 0] = UINT32_TYPE;
        data_[index_ + 1] = (value >>  0) & 0xFF;
        data_[index_ + 2] = (value >>  8) & 0xFF;
        data_[index_ + 3] = (value >> 16) & 0xFF;
        data_[index_ + 4] = (value >> 24) & 0xFF;

        index_ += 5;
    }

    void Write(const QString& value, Type type = STRING_TYPE)
    {
        const QChar* data = value.data();
        const int size = value.size();

        Preallocate(1);
        data_[index_ + 0] = type;
        ++index_;

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
    void Write(const QByteArray& value)
    {
        Preallocate(1);
        data_[index_ + 0] = BYTEARRAY_TYPE;
        ++index_;

        Write(value.size());
        Preallocate(value.size());

        for (int i = 0; i < value.size(); ++i)
        {
            data_[index_ + i] = value[i];
        }

        index_ += value.size();
    }
    void Write(qint64 value)
    {
        const int TYPE_SIZE = sizeof(value);

        Preallocate(TYPE_SIZE + 1);

        data_[index_ + 0] = INT64_TYPE;

        for (int i = 0; i < TYPE_SIZE; ++i)
        {
            data_[index_ + i + 1] = (value >> (i * 8)) & 0xFF;
        }

        index_ += (TYPE_SIZE + 1);
    }
    // Removed
    void Write(const char* value);
    void Preallocate(int size)
    {
        while ((index_ + size) > data_.size())
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
    bool IsEnd() const
    {
        if (index_ >= size_)
        {
            return true;
        }
        return false;
    }
    quint32 GetIndex() const
    {
        return index_;
    }
    void ReadType(QString* value)
    {
        Read(value, FastSerializer::TYPE_TYPE);
    }
    bool IsNextType(FastSerializer::Type type)
    {
        if (IsEnd())
        {
            return false;
        }
        if (data_[index_] != type)
        {
            return false;
        }
        return true;
    }
    FastSerializer::Type GetNextType() const
    {
        if (IsEnd())
        {
            kv::Abort("Cannot determine the next type because the end has been reached!");
        }
        return data_[index_];
    }
private:
    void Read(bool* value)
    {
        EnsureType(FastSerializer::BOOL_TYPE);
        EnsureSize(1);

        *value = data_[index_];
        ++index_;
    }

    void Read(qint32* value)
    {
        EnsureType(FastSerializer::INT32_TYPE);
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
        EnsureType(FastSerializer::UINT32_TYPE);
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

    void Read(
        QString* value,
        FastSerializer::Type type = FastSerializer::STRING_TYPE)
    {
        EnsureType(type);

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

    void Read(QByteArray* value)
    {
        EnsureType(FastSerializer::BYTEARRAY_TYPE);

        int size;
        Read(&size);

        EnsureSize(size);
        value->resize(size);

        char* data = value->data();

        for (int i = 0; i < size; ++i)
        {
            data[i] = data_[index_ + i];
        }

        index_ += size;
    }
    void Read(qint64* value)
    {
        const int TYPE_SIZE = sizeof(value);

        EnsureType(FastSerializer::INT64_TYPE);
        EnsureSize(TYPE_SIZE);

        *value = 0;

        quint64 helper;

        for (int i = 0; i < TYPE_SIZE; ++i)
        {
            helper = static_cast<unsigned char>(data_[index_ + i]);
            *value |= helper << (i * 8);
        }

        index_ += TYPE_SIZE;
    }
    void EnsureSize(int size)
    {
        if ((size + index_) > size_)
        {
            kv::Abort("FastDeserializer: EnsureSize fail!");
        }
    }
    void EnsureType(FastSerializer::Type type)
    {
        EnsureSize(1);
        if (data_[index_] != type)
        {
            const QString TEMPLATE("Types mismatch: expected - %1, actual - %2");
            kv::Abort(TEMPLATE.arg(static_cast<int>(type)).arg(static_cast<int>(data_[index_])));
        }
        ++index_;
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

inline QString Humanize(FastDeserializer* deserializer)
{
    QString retval;
    QTextStream stream(&retval);
    while (!deserializer->IsEnd())
    {
        FastSerializer::Type type = deserializer->GetNextType();
        if (type == FastSerializer::BOOL_TYPE)
        {
            bool value;
            *deserializer >> value;
            stream << value;
        }
        else if (type == FastSerializer::INT32_TYPE)
        {
            qint32 value;
            *deserializer >> value;
            stream << value;
        }
        else if (type == FastSerializer::UINT32_TYPE)
        {
            quint32 value;
            *deserializer >> value;
            stream << value;
        }
        else if (type == FastSerializer::STRING_TYPE)
        {
            QString value;
            *deserializer >> value;
            stream << value.replace('$', "\\$").replace(' ', "$");
        }
        else if (type == FastSerializer::BYTEARRAY_TYPE)
        {
            QByteArray value;
            *deserializer >> value;
            stream << value.toHex();
        }
        else if (type == FastSerializer::TYPE_TYPE)
        {
            QString value;
            deserializer->ReadType(&value);
            if (value == END_TYPE)
            {
                break;
            }
            stream << "\r\n" << value;
        }
        else
        {
            kv::Abort(QString("Unknown type: %1").arg(type));
        }
        stream << " ";
    }

    return retval;
}

}












