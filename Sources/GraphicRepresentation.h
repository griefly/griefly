#pragma once

#include <unordered_map>

#include "View.h"

class GraphicRepresentation
{
public:
    struct Entity
    {
        View view;
        size_t id;
        std::string name;
        int posx;
        int posy;
    };
    void Add(const Entity& ent)
    {

    }

    void Clear();
private:
    std::unordered_map<size_t, Entity>* old_frame_;
    std::unordered_map<size_t, Entity>* new_frame_;

    std::unordered_map<size_t, Entity> first_data_;
    std::unordered_map<size_t, Entity> seconds_data_;
};
