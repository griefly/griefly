#include "KvAbort.h"

#include <cstdlib>

#include <QDebug>

namespace kv
{

#ifdef _BUILD_COVER
extern "C" void __gcov_flush();
#endif // _BUILD_COVER
[[noreturn]] void Abort(const QString& message)
{
    qDebug() << message;
#ifdef _BUILD_COVER
    __gcov_flush();
#endif // _BUILD_COVER
    abort();
}

}
