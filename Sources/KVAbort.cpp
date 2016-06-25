#include <KVAbort.h>

#include <stdlib.h>

#ifdef _BUILD_COVER
extern "C" void __gcov_flush();
void kv_abort()
{
    __gcov_flush();
    abort();
}
#else
void kv_abort()
{
    abort();
}
#endif // _BUILD_COVER
