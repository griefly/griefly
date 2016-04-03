#include "Version.h"

#define KV_XSTR(x) #x
#define KV_STR(x) KV_XSTR(x)

const char* GetGameVersion()
{
    static const char* const VERSION = KV_STR(DEFINED_VERSION);
    return VERSION;
}
