#pragma once

#include <cstdlib>

#include <QDebug>
#include <QString>

namespace kv
{

#ifdef _BUILD_COVER
extern "C" void __gcov_flush();
#endif // _BUILD_COVER
[[noreturn]] inline void Abort(const QString& message)
{
    qDebug() << message;
#ifdef _BUILD_COVER
    __gcov_flush();
#endif // _BUILD_COVER
    abort();
}

}
