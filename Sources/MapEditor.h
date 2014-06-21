#pragma once

#include <list>

#include "HelperStructers.h"

class MapEditor
{
public:
    MapEditor();
    void InitWorld();
    void Run();

    int GetViewerX();
    int GetViewerY();
    int GetViewerZ();

    const std::list<point>* GetVisible() const
    {
        return visible_points_;
    }
private:
    void ProcessInput();
    void UpdateVisible();
    void DrawPointer();

    std::list<point>* visible_points_;

    int viewer_x_;
    int viewer_y_;
    int viewer_z_;

    int pointer_x_;
    int pointer_y_;
};

MapEditor* GetMapEditor();
void SetMapEditor(MapEditor* map_editor);