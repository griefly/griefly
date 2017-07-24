#include "KvAbort.h"

#include <cstdlib>

#include <QDebug>

#ifdef _BUILD_COVER
extern "C" void __gcov_flush();
#endif // _BUILD_COVER
[[noreturn]] void KvAbort(const QString& message)
{
    qDebug() << message;
#ifdef _BUILD_COVER
    __gcov_flush();
#endif // _BUILD_COVER
    abort();
}
