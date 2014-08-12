#pragma once

#include <vector>

unsigned int get_rand();

template<class T>
T pick(const std::vector<T>& values)
{
    unsigned int num = get_rand() % values.size();
    return values[num];
}

namespace random_helpers
{
    inline int random_shuffle(int v)
    {
        return static_cast<int>(get_rand() % v);
    }
    void set_rand(unsigned int seed, unsigned int calls_counter);
    unsigned int get_seed();
    unsigned int get_calls_counter();
}