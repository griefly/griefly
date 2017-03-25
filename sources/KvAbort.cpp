#include "KvAbort.h"

#include <cstdlib>

#include <QDebug>

#ifdef _BUILD_COVER
extern "C" void __gcov_flush();
void KvAbort(const QString& message)
{
    qDebug() << message;
    __gcov_flush();
    abort();
}
#else
void KvAbort(const QString& message)
{
    qDebug() << message;
    abort();
}
#endif // _BUILD_COVER
