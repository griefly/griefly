#pragma once

#include "Movable.h"

class Tables: public IMovable
{
public:
    DECLARE_SAVED(Tables, IMovable);
    DECLARE_GET_TYPE_ITEM(Tables);
    Tables(size_t id);
    virtual void AttackBy(id_ptr_on<Item> item) override;
    virtual void Delete() override;
    void CheckSurroundings(bool table);
    void UpdateSprite();
    void SetTable(Dir directon, bool table);
    virtual void AfterWorldCreation() override;
protected:
    std::string KV_SAVEBLE(material_);
private:
    bool KV_SAVEBLE(up_);
    bool KV_SAVEBLE(down_);
    bool KV_SAVEBLE(left_);
    bool KV_SAVEBLE(right_);
    bool KV_SAVEBLE(upright_);
    bool KV_SAVEBLE(downright_);
    bool KV_SAVEBLE(upleft_);
    bool KV_SAVEBLE(downleft_);
};
ADD_TO_TYPELIST(Tables);



class MetalTable: public Tables
{
public:
    DECLARE_SAVED(MetalTable, Tables);
    DECLARE_GET_TYPE_ITEM(MetalTable);
    MetalTable(size_t id);
}
ADD_TO_TYPELIST(MetalTable);
