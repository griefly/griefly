#pragma once

#include <sstream>
#include <string>

#include "constheader.h"

template<class T>
inline std::stringstream& WrapWriteMessage(std::stringstream& file, T& to_write)
{
    file << to_write;
    return file;
}

inline std::stringstream& WrapWriteMessage(std::stringstream& file, std::string& to_write)
{
    file << to_write.size() << " ";
    file << to_write;
    return file;
}

template<class T>
inline std::stringstream& WrapReadMessage(std::stringstream& file, T& to_read)
{
    file >> to_read;
    return file;
}

inline std::stringstream& WrapReadMessage(std::stringstream& file, std::string& to_read)
{
    size_t size;
    file >> size;

    char c;
    file.get(c);
    if (c != ' ')
    {
        SYSTEM_STREAM << "Error: corrupted string info" << std::endl;
        return file;
    }

    if (size == 0)
    {
        return file;
    }
    to_read.resize(size);
    for (size_t i = 0; i < size; ++i)
    {
        char cr;
        file.get(cr);
        to_read[i] = cr;
    }

  //  SYSTEM_STREAM << "Final string: '" << to_read << "'" << std::endl;

    return file;
}
