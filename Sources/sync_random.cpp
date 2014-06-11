#include <random>

#include "sync_random.h"
#include "constheader.h"

#include <memory>

typedef std::ranlux24 RandomGenerator;

RandomGenerator& GetGenerator(unsigned int seed = 0)
{
    static RandomGenerator* generator = new RandomGenerator(42);
    if (seed)
        generator->seed(seed);
    return *generator;
}

unsigned int calls_counter = 0;
unsigned int seed = 42;

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



