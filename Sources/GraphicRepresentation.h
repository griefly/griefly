#pragma once

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
    void Add(const Entity& ent);
    void Clear();
};
