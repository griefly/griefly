#pragma once

#include <sstream>
#include <QByteArray>

class FastStringstream
{
public:
    FastStringstream(int buffer_size);
    ~FastStringstream();
    void Reset();
    std::stringstream* GetStream();
    QByteArray GetCurrentData();
private:
    char* buffer_;
    std::stringstream stream_;
};
