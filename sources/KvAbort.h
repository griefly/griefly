#pragma once

#include <QString>

namespace kv
{

// __gcov_fflush + abort
void Abort(const QString& message);

}
