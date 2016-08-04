#pragma once

#include "Movable.h"

class Table: public IMovable
{
public:
    DECLARE_SAVED(Table, IMovable);
    DECLARE_GET_TYPE_ITEM(Table);
    Table(size_t id);
    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void Delete() override;
    void NotifyNeighborhood(bool is_in_existence);
    void UpdateSprite(size_t ignored_table);
    virtual void AfterWorldCreation() override;
protected:
    std::string KV_SAVEBLE(material_);
private:
    static int CheckTable(IdPtr<IOnMapBase> container, size_t ignored_table);
}
ADD_TO_TYPELIST(Table);



class MetalTable: public Table
{
public:
    DECLARE_SAVED(MetalTable, Table);
    DECLARE_GET_TYPE_ITEM(MetalTable);
    MetalTable(size_t id);
}
ADD_TO_TYPELIST(MetalTable);
