#include "GraphicRepresentation.h"

#include "constheader.h"
#include "helpers.h"

void GraphicRepresentation::Process()
{
    // TODO: mutex
    // Lock(mutex_);

    SynchronizeViews();

    Draw();

    // TODO: some time check
    PerformPixelMovement();
}

void GraphicRepresentation::SynchronizeViews()
{
    if (is_updated_)
    {
        for (auto it = current_frame_->begin(); it != current_frame_->end(); ++it)
        {
            auto& view_with_frame_id = views_[it->id];
            view_with_frame_id.view.LoadViewInfo(it->view);
            if (view_with_frame_id.frame_id != current_frame_id_)
            {
                view_with_frame_id.view.SetX(it->pixel_x);
                view_with_frame_id.view.SetY(it->pixel_y);
            }
            view_with_frame_id.frame_id = current_frame_id_ + 1;
        } 
        ++current_frame_id_;
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
const int MAXIMUM_PIXEL_SPEED = 16;
const int SPEED_DIVISOR = 4;

void GraphicRepresentation::PerformPixelMovement()
{
    for (auto it = current_frame_->begin(); it != current_frame_->end(); ++it)
    {
        auto& view_with_frame_id = views_[it->id];
        int old_x = view_with_frame_id.view.GetX();
        int old_y = view_with_frame_id.view.GetY();
        if ((old_x == it->pixel_x) && (old_y == it->pixel_y))
        {
            continue;
        }

        int diff_x = (it->pixel_x - old_x) / SPEED_DIVISOR;
        diff_x = limit_by_min_max_module(diff_x, MINIMUM_PIXEL_SPEED, MAXIMUM_PIXEL_SPEED);

        int diff_y = (it->pixel_y - old_y) / SPEED_DIVISOR;
        diff_y = limit_by_min_max_module(diff_y, MINIMUM_PIXEL_SPEED, MAXIMUM_PIXEL_SPEED);

        view_with_frame_id.view.SetX(old_x + diff_x);
        view_with_frame_id.view.SetY(old_y + diff_y);
    }
}

void GraphicRepresentation::Draw()
{
    for (int vlevel = 0; vlevel < MAX_LEVEL; ++vlevel)
    {
        for (auto it = current_frame_->begin(); it != current_frame_->end(); ++it)
        {
            if (it->vlevel == vlevel)
            {
                views_[it->id].view.Draw(helpers::dir_to_byond(it->dir));
            }
        }
    }
    for (auto it = current_frame_->begin(); it != current_frame_->end(); ++it)
    {
        if (it->vlevel >= MAX_LEVEL)
        {
            views_[it->id].view.Draw(helpers::dir_to_byond(it->dir));
        }
    }
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
