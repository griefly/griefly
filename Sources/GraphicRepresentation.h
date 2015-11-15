#pragma once

#include <unordered_map>
#include <vector>

#include "View.h"

class GraphicRepresentation
{
public:
    GraphicRepresentation()
    {
        old_frame_ = &first_data_;
        new_frame_ = &second_data_;
        is_updated_ = false;
    }

    struct Entity
    {
        View view;
        size_t id;
        int pos_x;
        int pos_y;
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
        std::swap(old_frame_, new_frame_);
        is_updated_ = true;
        new_frame_->clear();
    }
private:
    bool is_updated_;

    typedef std::vector<Entity> DataType;

    DataType* old_frame_;
    DataType* new_frame_;

    DataType first_data_;
    DataType second_data_;

    std::unordered_map<size_t, View> views_;
};
