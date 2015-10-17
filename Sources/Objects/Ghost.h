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
    virtual void processGUImsg(const Message2& msg) override;

    virtual void InitGUI() override;
    virtual void DeinitGUI() override;

    virtual void process() override;
private:
    int KV_SAVEBLE(seconds_until_respawn_);
    bool IsMobGhost();
};
ADD_TO_TYPELIST(Ghost);
