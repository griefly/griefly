#pragma once

#include <QString>

namespace kv
{

// __gcov_fflush + abort
[[noreturn]] void Abort(const QString& message);

}
