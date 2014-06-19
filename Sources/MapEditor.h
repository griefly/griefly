#pragma once

class MapEditor
{
public:
    MapEditor();
    void InitWorld();
    void Run();

    int GetViewerX();
    int GetViewerY();
private:
    int viewer_x_;
    int viewer_y_;

    int pointer_x_;
    int pointer_y_;
};

MapEditor* GetMapEditor();
void SetMapEditor(MapEditor* map_editor);