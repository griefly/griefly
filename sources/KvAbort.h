#pragma once

#include <QString>

// __gcov_fflush + abort
void KvAbort(const QString& message);
