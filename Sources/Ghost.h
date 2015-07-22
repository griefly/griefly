#pragma once

#include "MobInt.h"

class Ghost: public IMob
{
public:
    DECLARE_SAVED(Ghost, IMob);
    DECLARE_GET_TYPE_ITEM(Ghost);
    Ghost(size_t id);
    virtual void processImage(DrawType type) override;
    virtual bool IsTransp(int x, int y) override;
    virtual void CalculateVisible(std::list<point>* visible_list) override;
private:
    bool IsMobGhost();
};
ADD_TO_TYPELIST(Ghost);
