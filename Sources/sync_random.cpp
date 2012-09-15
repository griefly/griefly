#include <random>

#include "sync_random.h"

#include <memory>

typedef std::ranlux24 RandomGenerator;

RandomGenerator& GetGenerator(unsigned int seed = 0)
{
    static RandomGenerator* generator = new RandomGenerator(42);
    if (seed)
        generator->seed(seed);
    return *generator;
}

unsigned int calls_counter;
unsigned int seed;

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
        calls_counter = new_calls_counter;
        seed = new_seed;

        GetGenerator(new_seed).discard(new_calls_counter);
    }

    unsigned int get_seed()
    {
        return seed;
    }

    unsigned int get_calls_counter()
    {
        return calls_counter;
    }
}



