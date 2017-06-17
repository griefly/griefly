#pragma once

#include <random>

#include "objects/Object.h"

namespace kv
{

class SynchronizedRandom : public Object
{
public:
    DECLARE_SAVEABLE(SynchronizedRandom, Object);
    REGISTER_CLASS_AS(SynchronizedRandom);

    SynchronizedRandom();

    quint32 Generate();

    void SetParams(quint32 seed, quint32 calls_counter);
    quint32 GetSeed() const;
    quint32 GetCallsCounter() const;
private:
    quint32 KV_SAVEABLE(seed_);
    quint32 KV_SAVEABLE(calls_counter_);

    KV_ON_LOAD_CALL(InsertParamsIntoGenerator);
    void InsertParamsIntoGenerator();

    using RandomGenerator = std::ranlux24;
    RandomGenerator generator_;
};
END_DECLARE(SynchronizedRandom);

}
