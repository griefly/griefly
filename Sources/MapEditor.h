#pragma once

#include <list>
#include <hash_map>

#include "HelperStructers.h"
#include "OnMapInt.h"

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
    void SetAreaVars(int* xb, int* yb, int* zb,
                     int* xe, int* ye, int* ze);

    void InitCreationList();

    void ProcessInput();
    void UpdateVisible();
    void DrawPointer();
    void NormalizePointer();
    void DrawChoosenItem();
    void UpdateCoords();

    void SaveMap(std::stringstream& savefile);

    std::vector<IOnMapObject*> for_creation_;
    int to_create_;

    std::list<point>* visible_points_;

    int viewer_x_;
    int viewer_y_;
    int viewer_z_;

    int pointer_x_;
    int pointer_y_;

    point point1_;
    point point2_;
};

MapEditor* GetMapEditor();
void SetMapEditor(MapEditor* map_editor);