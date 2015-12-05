#include "GraphicRepresentation.h"

#include "constheader.h"
#include "helpers.h"

void GraphicRepresentation::Process()
{
    // TODO: mutex
    // Lock(mutex_);

    SynchronizeViews();

    Draw();

    const int PIXEL_MOVEMENT_SPEED = 20;

    if ((SDL_GetTicks() - pixel_movement_tick_) > PIXEL_MOVEMENT_SPEED)
    {
        PerformPixelMovement();
        pixel_movement_tick_ = SDL_GetTicks();
    }
}

void GraphicRepresentation::SynchronizeViews()
{
    if (is_updated_)
    {
        // TODO: pixel shift in camera
        camera_.pos_x = current_frame_->camera_pos_x;
        camera_.pos_y = current_frame_->camera_pos_y;

        for (auto it = current_frame_->entities.begin(); it != current_frame_->entities.end(); ++it)
        {
            auto& view_with_frame_id = views_[it->id];
            view_with_frame_id.view.LoadViewInfo(it->view);
            if (view_with_frame_id.frame_id != current_frame_id_)
            {
                view_with_frame_id.view.SetX(it->pos_x * 32);
                view_with_frame_id.view.SetY(it->pos_y * 32);
            }
            view_with_frame_id.frame_id = current_frame_id_ + 1;
        } 

        ++current_frame_id_;
        is_updated_ = false;
    }
}

int limit_by_min_max_module(int value, int min, int max)
{
    if (value > max)
    {
        value = max;
    }
    if ((value > 0) && (value < min))
    {
        value = min;
    }
    if ((value < 0) && (value > -1 * min))
    {
        value = -1 * min;
    }
    if (value < -1 * max)
    {
        value = -1 * max;
    }
    return value;
}

const int MINIMUM_PIXEL_SPEED = 1;
const int MAXIMUM_PIXEL_SPEED = 6;

void GraphicRepresentation::PerformPixelMovement()
{
    for (auto it = current_frame_->entities.begin(); it != current_frame_->entities.end(); ++it)
    {
        int pixel_x = it->pos_x * 32;
        int pixel_y = it->pos_y * 32;

        auto& view_with_frame_id = views_[it->id];
        int old_x = view_with_frame_id.view.GetX();
        int old_y = view_with_frame_id.view.GetY();
        if (   old_x == pixel_x
            && old_y == pixel_y)
        {
            continue;
        }

        int diff_x = pixel_x - old_x;
        diff_x = limit_by_min_max_module(diff_x, MINIMUM_PIXEL_SPEED, MAXIMUM_PIXEL_SPEED);

        int diff_y = pixel_y - old_y;
        diff_y = limit_by_min_max_module(diff_y, MINIMUM_PIXEL_SPEED, MAXIMUM_PIXEL_SPEED);

        view_with_frame_id.view.SetX(old_x + diff_x);
        view_with_frame_id.view.SetY(old_y + diff_y);
    }
}

void GraphicRepresentation::Draw()
{
    for (int vlevel = 0; vlevel < MAX_LEVEL; ++vlevel)
    {
        for (auto it = current_frame_->entities.begin(); it != current_frame_->entities.end(); ++it)
        {
            if (it->vlevel == vlevel)
            {
                views_[it->id].view.Draw(
                    camera_.GetFullShiftX(),
                    camera_.GetFullShiftY(),
                    helpers::dir_to_byond(it->dir));
            }
        }
    }
    for (auto it = current_frame_->entities.begin(); it != current_frame_->entities.end(); ++it)
    {
        if (it->vlevel >= MAX_LEVEL)
        {
            views_[it->id].view.Draw(
                camera_.GetFullShiftX(),
                camera_.GetFullShiftY(),
                helpers::dir_to_byond(it->dir));
        }
    }
}


int GraphicRepresentation::CameraData::GetFullShiftX()
{
    return -1 * (pos_x * 32 + pixel_shift_x_) + (sizeW / 2);
}

int GraphicRepresentation::CameraData::GetFullShiftY()
{
    return -1 * (pos_y * 32 + pixel_shift_y_) + (sizeH / 2);
}

GraphicRepresentation* g_r = nullptr;
GraphicRepresentation& GetGraphicRepresentation()
{
    return *g_r;
}

void SetGraphicRepresentation(GraphicRepresentation* new_g_r)
{
    g_r = new_g_r;
}
