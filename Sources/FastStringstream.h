#pragma once

#include <sstream>
#include <QByteArray>

class FastStringstream
{
public:
    static const int DEFAULT_BUFFER_SIZE = 1024 * 1024 * 32;
    FastStringstream(int buffer_size = DEFAULT_BUFFER_SIZE);
    ~FastStringstream();
    void Reset();
    std::stringstream* GetStream();
    QByteArray GetCurrentData();
private:
    char* buffer_;
    std::stringstream stream_;
};
