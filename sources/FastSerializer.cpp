#include "FastSerializer.h"

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

FastSerializer::FastSerializer(int size)
    : index_(0)
{
    data_.resize(size);
}

FastSerializer::~FastSerializer()
{
    // Nothing
}

QString Humanize(FastDeserializer *deserializer)
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
            qDebug() << "Unknown type: " << type;
            KvAbort();
        }
        stream << " ";
    }

    return retval;
}
