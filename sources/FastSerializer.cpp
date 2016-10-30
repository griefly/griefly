#include "FastSerializer.h"

static_assert(
    sizeof(int) == 4,
    "Unsupported platform, sizeof(int) should be 4!");
static_assert(
    sizeof(unsigned int) == 4,
    "Unsupported platform, sizeof(unsigned int) should be 4!");
static_assert(
    sizeof(char) == 1,
    "Unsupported platform, sizeof(char) should be 1!");
static_assert(
    sizeof(bool) == 1,
    "Unsupported platform, sizeof(bool) should be 1!");

FastSerializer::FastSerializer()
{
    const int DEFAULT_SIZE = 32 * 1024 * 1024;
    data_.resize(DEFAULT_SIZE);

    index_ = 0;
}

FastSerializer::~FastSerializer()
{
    // Nothing
}
