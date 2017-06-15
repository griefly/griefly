#pragma once 

#include "Item.h"

namespace kv
{

class Shard : public Item
{
public:
    DECLARE_SAVEABLE(Shard, Item);
    REGISTER_CLASS_AS(Shard);
    Shard();
    virtual void AfterWorldCreation() override;
};
END_DECLARE(Shard);

}
