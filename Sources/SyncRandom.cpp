#include <random>

#include "SyncRandom.h"
#include "Constheader.h"

#include <memory>

#include <stdlib.h>

typedef std::ranlux24 RandomGenerator;

unsigned int seed = rand();

RandomGenerator& GetGenerator(unsigned int seed_loc = 0)
{
    static RandomGenerator* generator = new RandomGenerator(seed);
    if (seed_loc)
        generator->seed(seed_loc);
    return *generator;
}

unsigned int calls_counter = 0;

unsigned int get_rand()
{
    ++calls_counter;
    unsigned int retval = GetGenerator()();
    return retval;
}

namespace random_helpers
{
    void set_rand(unsigned int new_seed, unsigned int new_calls_counter)
    {
        SYSTEM_STREAM << "set_seed: " << new_seed << std::endl;
        SYSTEM_STREAM << "set_calls_counter: " << new_calls_counter << std::endl;
        calls_counter = new_calls_counter;
        seed = new_seed;

        GetGenerator(new_seed).discard(new_calls_counter);
    }

    unsigned int get_seed()
    {
        SYSTEM_STREAM << "get_seed: " << seed << std::endl;
        return seed;
    }

    unsigned int get_calls_counter()
    {
        SYSTEM_STREAM << "get_calls_counter: " << calls_counter << std::endl;
        return calls_counter;
    }
}



