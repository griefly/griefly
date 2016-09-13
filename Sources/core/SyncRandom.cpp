#include <random>

#include "SyncRandom.h"
#include "Constheader.h"

#include <memory>

#include <cstdlib>

SyncRandom::SyncRandom()
    : seed_(rand()),
      calls_counter_(0),
      generator_(seed_)
{

}

unsigned int SyncRandom::GetRand()
{
    ++calls_counter_;
    unsigned int retval = generator_();
    return retval;
}

void SyncRandom::SetRand(unsigned int new_seed, unsigned int new_calls_counter)
{
    SYSTEM_STREAM << "set_seed: " << new_seed << std::endl;
    SYSTEM_STREAM << "set_calls_counter: " << new_calls_counter << std::endl;
    calls_counter_ = new_calls_counter;
    seed_ = new_seed;

    generator_.seed(new_seed);
    generator_.discard(new_calls_counter);
}

unsigned int SyncRandom::GetSeed()
{
    SYSTEM_STREAM << "get_seed: " << seed_ << std::endl;
    return seed_;
}

unsigned int SyncRandom::GetCallsCounter()
{
    SYSTEM_STREAM << "get_calls_counter: " << calls_counter_ << std::endl;
    return calls_counter_;
}
