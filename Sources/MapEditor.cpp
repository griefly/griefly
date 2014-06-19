#include "MapEditor.h"

#include "OnMapBaseInt.h"
#include "Screen.h"
#include "mob_position.h"
#include "SdlInit.h"
#include "ItemFabric.h"
#include "MapClass.h"

MapEditor::MapEditor()
{
    viewer_x_ = 0;
    viewer_y_ = 0;

    pointer_x_ = 0;
    pointer_y_ = 0;
}

void MapEditor::InitWorld()
{
    if (!InitSDL())
    {
        SYSTEM_STREAM << "Fail to load SDL" << std::endl;
    }
    SDL_WM_SetCaption("Map Editor Mode", "Map Editor Mode");

    SetItemFabric(new ItemFabric);
    SetMapMaster(new MapMaster);
    SetScreen(new Screen(sizeW, sizeH));
    SetTexts(new TextPainter);
    SetSpriter(new ASprClass);

    SetMapEditor(this);
}

void MapEditor::Run()
{
}

int MapEditor::GetViewerX()
{
    return viewer_x_;
}

int MapEditor::GetViewerY()
{
    return viewer_y_;
}

MapEditor* map_editor_ = nullptr;
MapEditor* GetMapEditor()
{
    return map_editor_;
}

void SetMapEditor(MapEditor* map_editor)
{
    map_editor_ = map_editor;
}
