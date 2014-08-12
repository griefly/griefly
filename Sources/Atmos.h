#pragma once

#include <array>
#include <vector>

#include "AtmosHolder.h"
#include "constheader.h"

class Atmosphere
{
public:
    void Resize(size_t x, size_t y)
    {
        x_shuffle_.resize(x);
        y_shuffle_.resize(y);

        for (size_t i = 0; i < dir_shuffle_.size(); ++i)
            dir_shuffle_[i] = i;
        for (size_t i = 0; i < x_shuffle_.size(); ++i)
            x_shuffle_[i] = i;
        for (size_t i = 0; i < y_shuffle_.size(); ++i)
            y_shuffle_[i] = i;
    }
    void Process();
private:
    void ProcessTile(size_t x, size_t y, size_t z); 

    void ShuffleX();
    void ShuffleY();
    void ShuffleDir();
    std::vector<size_t> x_shuffle_;
    std::vector<size_t> y_shuffle_;

    std::array<Dir, 4> dir_shuffle_;
};