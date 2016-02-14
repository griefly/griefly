#include "FastStringstream.h"

#include <QDebug>

FastStringstream::FastStringstream(int buffer_size)
{
    buffer_ = new char[buffer_size];
    //stream_.rdbuf()->pubsetbuf(buffer_, buffer_size);
}

FastStringstream::~FastStringstream()
{
    delete[] buffer_;
}

void FastStringstream::Reset()
{
    //stream_.clear();
    //stream_.seekp(0);
    stream_.str("");
}

std::stringstream* FastStringstream::GetStream()
{
    return &stream_;
}

QByteArray FastStringstream::GetCurrentData()
{
    if (!stream_.good())
    {
        return "";
    }
    qDebug() << "Tellp: " << stream_.tellp();
    std::string str = stream_.str();
    QByteArray retval(str.c_str(), str.length());
    return retval;
}
