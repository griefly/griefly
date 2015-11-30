#pragma once

#include <unordered_map>
#include <vector>

#include "View2.h"
#include "ViewInfo.h"

class GraphicRepresentation
{
public:
    GraphicRepresentation()
    {
        current_frame_ = &first_data_;
        new_frame_ = &second_data_;
        is_updated_ = false;
        current_frame_id_ = 0;
    }

    struct Entity
    {
        ViewInfo view;
        size_t id;
        int pixel_x;
        int pixel_y;
        int vlevel;
        int dir;
    };
    void AddToNewFrame(const Entity& ent)
    {
        new_frame_->push_back(ent);
    }
    void Swap()
    {
        // TODO: mutex
        // Lock(frames_mutex_);
        std::swap(current_frame_, new_frame_);
        is_updated_ = true;
        new_frame_->clear();
    }
    void Process();
private:
    void SynchronizeViews();
    void PerformPixelMovement();

    size_t current_frame_id_;

    bool is_updated_;

    typedef std::vector<Entity> DataType;

    DataType* current_frame_;
    DataType* new_frame_;

    DataType first_data_;
    DataType second_data_;

    struct ViewWithFrameId
    {
        View2 view;
        size_t frame_id;
    };

    std::unordered_map<size_t, ViewWithFrameId> views_;
};
