#include "FastSerializer.h"

static_assert(
    sizeof(int) == 4,
    "Unsupported platform, sizeof(int) should be 4!");
static_assert(
    sizeof(unsigned int) == 4,
    "Unsupported platform, sizeof(unsigned int) should be 4!");
static_assert(
    sizeof(char) == 1,
    "Unsupported platform, sizeo(char) should be 1!");
static_assert(
    sizeof(bool) == 1,
    "Unsupported platform, sizeof(bool) should be 1!");

FastSerializer::FastSerializer()
{

}
