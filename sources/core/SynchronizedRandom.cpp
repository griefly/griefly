#include "SynchronizedRandom.h"

using namespace kv;

SynchronizedRandom::SynchronizedRandom()
    : seed_(qrand()),
      calls_counter_(0),
      generator_(seed_)
{
    // Nothing
}

quint32 SynchronizedRandom::Generate()
{
    ++calls_counter_;
    quint32 retval = generator_();
    return retval;
}

void SynchronizedRandom::SetParams(quint32 new_seed, quint32 new_calls_counter)
{
    calls_counter_ = new_calls_counter;
    seed_ = new_seed;

    InsertParamsIntoGenerator();
}

quint32 SynchronizedRandom::GetSeed() const
{
    return seed_;
}

quint32 SynchronizedRandom::GetCallsCounter() const
{
    return calls_counter_;
}

void SynchronizedRandom::InsertParamsIntoGenerator()
{
    generator_.seed(seed_);
    generator_.discard(calls_counter_);
}
