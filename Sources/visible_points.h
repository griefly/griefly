#pragma once

#include <list>

#include "HelperStructers.h"

const std::list<point>* GetVisible();
bool IsTileVisible(size_t tile_id);
