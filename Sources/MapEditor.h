#pragma once

#include "Screen.h"

class MapEditor
{
public:
    void InitWorld();
    void Run();

    int GetViewerX();
    int GetViewerY();
private:
    int viewer_x_;
    int viewer_y_;
};