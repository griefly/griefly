#include "Version.h"

#define KV_XSTR(x) #x
#define KV_STR(x) KV_XSTR(x)

const char* get_game_version()
{
    static const char* const VERSION = KV_STR(DEFINED_VERSION);
    if (VERSION[0] == '\0')
    {
        return "v0.0.0-unknown";
    }
    return VERSION;
}
