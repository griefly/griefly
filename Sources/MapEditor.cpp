#include "MapEditor.h"

#include "OnMapBaseInt.h"
#include "Screen.h"
#include "mob_position.h"
#include "SdlInit.h"
#include "ItemFabric.h"
#include "MapClass.h"
#include "Text.h"
#include "LiquidHolder.h"

MapEditor::MapEditor()
{
    viewer_x_ = sizeHmap / 2;
    viewer_y_ = sizeWmap / 2;
    viewer_z_ = sizeDmap / 2;

    pointer_x_ = viewer_x_;
    pointer_y_ = viewer_y_;

    visible_points_ = new std::list<point>;
}

void MapEditor::InitWorld()
{
    if (!InitSDL())
    {
        SYSTEM_STREAM << "Fail to load SDL" << std::endl;
    }
    SDL_WM_SetCaption("Map Editor Mode", "Map Editor Mode");

    SetMapEditor(this);

    SetItemFabric(new ItemFabric);
    SetMapMaster(new MapMaster);
    SetScreen(new Screen(sizeW, sizeH));
    SetTexts(new TextPainter);
    SetSpriter(new ASprClass);

    GetMapMaster()->makeTiles();
    srand(SDL_GetTicks());
    GetMapMaster()->makeMap();

    LiquidHolder::LoadReaction();
}

void MapEditor::Run()
{
    while (true)
    {
        ProcessInput();
        UpdateVisible();
        GetMapMaster()->Draw();
        DrawPointer();
        GetScreen()->Swap();
    }
}

void MapEditor::ProcessInput()
{
    SDL_Event event;    
    while (SDL_PollEvent(&event))
    { 
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            auto item = GetMapMaster()->click(event.button.x, event.button.y);
        }
        if (event.type == SDL_VIDEORESIZE)
        {
            int max_scale = std::max((event.resize.w / 4), (event.resize.h / 3));

            int new_w = max_scale * 4;
            int new_h = max_scale * 3;
            GetScreen()->ResetScreen(new_w, new_h, 32, SDL_OPENGL | SDL_RESIZABLE);
        }
    }
}

void MapEditor::DrawPointer()
{

}

void MapEditor::UpdateVisible()
{
    visible_points_->clear();
    point p;
    p.posz = viewer_z_;
    for (int i = viewer_x_ - sizeHsq + 1; i < viewer_x_ + sizeHsq; ++i)
        for (int j = viewer_y_ - sizeWsq + 1; j < viewer_y_ + sizeWsq; ++j)
        {
            p.posx = i;
            p.posy = j;
            visible_points_->push_back(p);
        }

}

int MapEditor::GetViewerX()
{
    return viewer_x_;
}

int MapEditor::GetViewerY()
{
    return viewer_y_;
}

int MapEditor::GetViewerZ()
{
    return viewer_z_;
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
