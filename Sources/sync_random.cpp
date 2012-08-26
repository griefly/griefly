#include <random>

#include "sync_random.h"

typedef std::ranlux24 RandomGenerator;

RandomGenerator& GetGenerator(unsigned int seed = 0)
{
    static RandomGenerator* generator = new RandomGenerator(42);
    if (seed)
        generator->seed(seed);
    return *generator;
}

unsigned int get_rand()
{
    unsigned int retval = GetGenerator()();
    return retval;
}
void set_rand(unsigned int seed)
{
    GetGenerator(seed);
}

