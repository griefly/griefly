#include "MapEditor.h"

#include "OnMapBaseInt.h"
#include "Screen.h"
#include "mob_position.h"
#include "SdlInit.h"
#include "ItemFabric.h"
#include "MapClass.h"
#include "Text.h"
#include "LiquidHolder.h"
#include "helpers.h"
#include "Params.h"
#include "MobInt.h"
#include "Creator.h"

#include "utils.h"

MapEditor::MapEditor()
{
    to_create_ = 0;

    visible_points_ = new std::list<point>;
    
    point1_.posx = -1;
    point1_.posy = -1;
    point1_.posz = -1;

    point2_.posx = -1;
    point2_.posy = -1;
    point2_.posz = -1;

    for (auto it = (*itemList()).begin(); it != (*itemList()).end(); ++it)
    {
        IMainItem* loc = it->second();
        IOnMapItem* bloc = castTo<IOnMapItem>(loc);
        if (!bloc)
        {
            delete loc;
            continue;
        }
        for_creation_.push_back(bloc);
    }
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

    srand(SDL_GetTicks());
    
    int x = GetParamsHolder().GetParamBool("map_x") ? 
            GetParamsHolder().GetParam<int>("map_x") : 40;
    int y = GetParamsHolder().GetParamBool("map_y") ? 
            GetParamsHolder().GetParam<int>("map_y") : 40;
    int z = GetParamsHolder().GetParamBool("map_z") ? 
            GetParamsHolder().GetParam<int>("map_z") : 2;

    GetMapMaster()->makeTiles(x, y, z);

    if (   !GetParamsHolder().GetParamBool("map_name") 
        || !utils::IsFileExist(GetParamsHolder().GetParam<std::string>("map_name")))
    {

        auto newmob = GetItemFabric()->newItemOnMap<IMob>(
                hash("ork"), 
                GetMapMaster()->squares[GetMapMaster()->GetMapW() / 2][GetMapMaster()->GetMapH() / 2][1]);
        SetMob(newmob.ret_id());
        GetItemFabric()->SetPlayerId(newmob.ret_id(), newmob.ret_id());

        auto tptr = GetItemFabric()->newItemOnMap<IOnMapItem>(
                hash("Teleportator"), 
                GetMapMaster()->squares[GetMapMaster()->GetMapW() / 2][GetMapMaster()->GetMapH() / 2][1]);
        SetCreator(tptr.ret_id());

        //GetMapMaster()->makeMap();
    }
    else
    {
       std::string str = GetParamsHolder().GetParam<std::string>("map_name");
       GetItemFabric()->loadMap(str.c_str());
    }
    UpdateCoords();
    LiquidHolder::LoadReaction();

    GetTexts()["ToCreate"].SetUpdater
    ([this](std::string* str)
    {
        std::stringstream ss; 
        ss << for_creation_[to_create_]->name; 
        ss >> *str;
    }).SetFreq(10).SetSize(15).SetPlace(16, 16, 16 + 32 * 3);
}

void MapEditor::UpdateCoords()
{
    viewer_x_ = GetMapMaster()->GetMapW() / 2;
    viewer_y_ = GetMapMaster()->GetMapH() / 2;
    viewer_z_ = GetMapMaster()->GetMapD() / 2;

    pointer_x_ = viewer_x_;
    pointer_y_ = viewer_y_;
}

void MapEditor::Run()
{
    while (true)
    {
        ProcessInput();
        UpdateVisible();
        GetMapMaster()->Draw();
        DrawPointer();
        DrawChoosenItem();
        GetTexts().Process();
        GetScreen()->Swap();
    }
}

void MapEditor::DrawChoosenItem()
{
    const GLSprite* spr = GetSpriter()->returnSpr("icons/background.png");
    GetScreen()->Draw(spr, 16 + 32 * 0, 16, 0, 0);
    GetScreen()->Draw(spr, 16 + 32 * 1, 16, 0, 0);

    GetScreen()->Draw(spr, 16 + 32 * 0, 48, 0, 0);
    GetScreen()->Draw(spr, 16 + 32 * 1, 48, 0, 0);

    spr = for_creation_[to_create_]->GetSprite();
    if (!spr->Fail())
    {
        GetScreen()->Draw(spr, 30, 32, 0, 0);
    }
}

void MapEditor::SaveMap(std::stringstream& savefile)
{
}

void MapEditor::NormalizePointer()
{
    // TODO
}

void MapEditor::SetAreaVars(int* xb, int* yb, int* zb,
                            int* xe, int* ye, int* ze)
{
    int x_begin = point1_.posx;
    int y_begin = point1_.posy;
    int z_begin = point1_.posz;

    int x_end = point2_.posx;
    int y_end = point2_.posy;
    int z_end = point2_.posz;

    if (x_begin == -1 || x_end == -1)
    {
        x_begin = pointer_x_;
        y_begin = pointer_y_;
        z_begin = viewer_z_;

        x_end = pointer_x_;
        y_end = pointer_y_;
        z_end = viewer_z_;
    }

    if (x_end < x_begin)
        std::swap(x_end, x_begin);
    if (y_end < y_begin)
        std::swap(y_end, y_begin);
    if (z_end < z_begin)
        std::swap(z_end, z_begin);

    *xb = x_begin;
    *xe = x_end;

    *yb = y_begin;
    *ye = y_end;

    *zb = z_begin;
    *ze = z_end;
}

void MapEditor::ProcessInput()
{
    SDL_Event event;    
    while (SDL_PollEvent(&event))
    { 
        if(event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_w)
            {
                helpers::move_to_dir(D_UP, &pointer_x_, &pointer_y_);
            }
            else if (event.key.keysym.sym == SDLK_s)
            {
                helpers::move_to_dir(D_DOWN, &pointer_x_, &pointer_y_);
            }
            else if (event.key.keysym.sym == SDLK_d)
            {
                helpers::move_to_dir(D_RIGHT, &pointer_x_, &pointer_y_);
            }
            else if (event.key.keysym.sym == SDLK_a)
            {
                helpers::move_to_dir(D_LEFT, &pointer_x_, &pointer_y_);
            }
            else if (event.key.keysym.sym == SDLK_UP)
            {
                helpers::move_to_dir(D_UP, &viewer_x_, &viewer_y_);
                helpers::move_to_dir(D_UP, &pointer_x_, &pointer_y_);
            }
            else if (event.key.keysym.sym == SDLK_DOWN)
            {
                helpers::move_to_dir(D_DOWN, &viewer_x_, &viewer_y_);
                helpers::move_to_dir(D_DOWN, &pointer_x_, &pointer_y_);
            }
            else if (event.key.keysym.sym == SDLK_RIGHT)
            {
                helpers::move_to_dir(D_RIGHT, &viewer_x_, &viewer_y_);
                helpers::move_to_dir(D_RIGHT, &pointer_x_, &pointer_y_);
            }
            else if (event.key.keysym.sym == SDLK_LEFT)
            {
                helpers::move_to_dir(D_LEFT, &viewer_x_, &viewer_y_);
                helpers::move_to_dir(D_LEFT, &pointer_x_, &pointer_y_);
            }
            else if (event.key.keysym.sym == SDLK_COMMA)
            {
                helpers::move_to_dir(D_ZDOWN, &viewer_x_, &viewer_y_, &viewer_z_);
            }
            else if (event.key.keysym.sym == SDLK_PERIOD)
            {
                helpers::move_to_dir(D_ZUP, &viewer_x_, &viewer_y_, &viewer_z_);
            }
            else if (event.key.keysym.sym == SDLK_e)
            {
                SYSTEM_STREAM << "SDLK_e: " << std::endl;
                to_create_ = std::min(static_cast<int>(for_creation_.size() - 1), to_create_ + 1);
                SYSTEM_STREAM << "to_create_: " << to_create_ << std::endl;
                SYSTEM_STREAM << for_creation_[to_create_]->T_SPR << std::endl;
                SYSTEM_STREAM << for_creation_[to_create_]->name << std::endl;
                SYSTEM_STREAM << "=========" << std::endl;
            }
            else if (event.key.keysym.sym == SDLK_q)
            {
                SYSTEM_STREAM << "SDLK_q" << std::endl;
                to_create_ = std::max(0, to_create_ - 1);
                SYSTEM_STREAM << "to_create_: " << to_create_ << std::endl;
                SYSTEM_STREAM << for_creation_[to_create_]->T_SPR << std::endl;
                SYSTEM_STREAM << for_creation_[to_create_]->name << std::endl;
                SYSTEM_STREAM << "=========" << std::endl;
            }
            else if (event.key.keysym.sym == SDLK_SPACE)
            {
                int x_begin, y_begin, z_begin;
                int x_end, y_end, z_end;
                SetAreaVars(&x_begin, &y_begin, &z_begin,
                            &x_end,   &y_end,   &z_end);

                for (int counter_x = x_begin; counter_x <= x_end; ++counter_x)
                    for (int counter_y = y_begin; counter_y <= y_end; ++counter_y)
                        for (int counter_z = z_begin; counter_z <= z_end; ++counter_z)
                            if (!castTo<ITurf>(for_creation_[to_create_]))
                                GetItemFabric()->newItemOnMap<IOnMapItem>(
                                    for_creation_[to_create_]->T_ITEM(), 
                                    GetMapMaster()->squares[counter_x][counter_y][counter_z]);
            }
            else if (event.key.keysym.sym == SDLK_t)
            {
                int x_begin, y_begin, z_begin;
                int x_end, y_end, z_end;
                SetAreaVars(&x_begin, &y_begin, &z_begin,
                            &x_end,   &y_end,   &z_end);

                for (int counter_x = x_begin; counter_x <= x_end; ++counter_x)
                    for (int counter_y = y_begin; counter_y <= y_end; ++counter_y)
                        for (int counter_z = z_begin; counter_z <= z_end; ++counter_z)
                        {
                            auto current_tile = GetMapMaster()->squares[counter_x][counter_y][counter_z];
                            if (current_tile->GetTurf().valid())
                            {
                                current_tile->GetTurf()->delThis();
                            }
                            else if (castTo<ITurf>(for_creation_[to_create_]))
                            {
                                auto trf = GetItemFabric()->newItem<ITurf>(for_creation_[to_create_]->T_ITEM());
                                current_tile->SetTurf(trf);
                            }
                        }
            }
            else if (event.key.keysym.sym == SDLK_F5)
            {
                GetItemFabric()->saveMap(
                    GetParamsHolder().GetParamBool("map_name") ?
                    GetParamsHolder().GetParam<std::string>("map_name").c_str() :
                    "default.map");
                SYSTEM_STREAM << "Map saved" << std::endl;
            }
            else if (event.key.keysym.sym == SDLK_DELETE)
            {
                int x_begin, y_begin, z_begin;
                int x_end, y_end, z_end;
                SetAreaVars(&x_begin, &y_begin, &z_begin,
                            &x_end,   &y_end,   &z_end);

                for (int counter_x = x_begin; counter_x <= x_end; ++counter_x)
                    for (int counter_y = y_begin; counter_y <= y_end; ++counter_y)
                        for (int counter_z = z_begin; counter_z <= z_end; ++counter_z)
                        {
                            auto ptr = GetMapMaster()->squares[counter_x][counter_y][counter_z]->GetItem<IOnMapItem>();
                            if (ptr.valid())
                                ptr->delThis();
                            else if (GetMapMaster()->squares[counter_x][counter_y][counter_z]->GetTurf().valid())
                                GetMapMaster()->squares[counter_x][counter_y][counter_z]->GetTurf()->delThis();
                        }
            }
            else if (event.key.keysym.sym == SDLK_RSHIFT)
            {
                if (point1_.posx == -1)
                {
                    point1_.posx = pointer_x_;
                    point1_.posy = pointer_y_;
                    point1_.posz = viewer_z_;
                }
                else
                {
                    point1_.posx = -1;
                    point1_.posy = -1;
                    point1_.posz = -1;
                }
            }
            else if (event.key.keysym.sym == SDLK_LSHIFT)
            {
                if (point2_.posx == -1)
                {
                    point2_.posx = pointer_x_;
                    point2_.posy = pointer_y_;
                    point2_.posz = viewer_z_;
                }
                else
                {
                    point2_.posx = -1;
                    point2_.posy = -1;
                    point2_.posz = -1;
                }
            }
        }
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
    auto spr = GetSpriter()->returnSpr("icons/editor_pointer.png");
    GetScreen()->Draw(spr, 
                      pointer_x_ * 32 + mob_position::get_shift_x(),
                      pointer_y_ * 32 + mob_position::get_shift_y(),
                      0, 0);

    spr = GetSpriter()->returnSpr("icons/up_left_point.png");
    if (viewer_z_ == point1_.posz)
        GetScreen()->Draw(spr, 
                          point1_.posx * 32 + mob_position::get_shift_x(),
                          point1_.posy * 32 + mob_position::get_shift_y(),
                          0, 0);

    spr = GetSpriter()->returnSpr("icons/down_right_point.png");
    if (viewer_z_ == point2_.posz)
        GetScreen()->Draw(spr, 
                          point2_.posx * 32 + mob_position::get_shift_x(),
                          point2_.posy * 32 + mob_position::get_shift_y(),
                          0, 0);
}

void MapEditor::UpdateVisible()
{
    visible_points_->clear();
    point p;
    p.posz = viewer_z_;
    int x_low_border = std::max(0, viewer_x_ - sizeHsq + 1);
    int x_high_border = std::min(GetMapMaster()->GetMapW(), viewer_x_ + sizeHsq);
    int y_low_border = std::max(0, viewer_y_ - sizeWsq + 1);
    int y_high_border = std::max(GetMapMaster()->GetMapH(), viewer_y_ + sizeWsq);
    for (int i = x_low_border; i < x_high_border; ++i)
        for (int j = y_low_border; j < y_high_border; ++j)
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
