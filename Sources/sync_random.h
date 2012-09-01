#pragma once

unsigned int get_rand();

namespace random_helpers
{
    void set_rand(unsigned int seed, unsigned int calls_counter);
    unsigned int get_seed();
    unsigned int get_calls_counter();
}