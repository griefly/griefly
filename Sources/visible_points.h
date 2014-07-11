#pragma once

#include <stdio.h>  

#include <list>

#include "HelperStructers.h"

const std::list<point>* GetVisible();
bool IsTileVisible(size_t tile_id);
