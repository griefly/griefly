#include "FastStringstream.h"

FastStringstream::FastStringstream(int buffer_size)
{
    buffer_ = new char[buffer_size];
    stream_.rdbuf()->pubsetbuf(buffer_, buffer_size);
}

FastStringstream::~FastStringstream()
{
    delete[] buffer_;
}

void FastStringstream::Reset()
{
    stream_.clear();
    stream_.seekp(0);
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
    QByteArray retval(buffer_, stream_.tellp());
    return retval;
}
