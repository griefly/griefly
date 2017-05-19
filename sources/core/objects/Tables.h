#pragma once

#include "Movable.h"

namespace kv
{

class Table : public Movable
{
public:
    DECLARE_SAVEABLE(Table, Movable);
    REGISTER_CLASS_AS(Table);
    Table();
    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void Delete() override;
    void NotifyNeighborhood(bool is_in_existence);
    void UpdateSprite(quint32 ignored_table);
    virtual void AfterWorldCreation() override;
protected:
    QString KV_SAVEABLE(material_);
private:
    static int CheckTable(IdPtr<MapObject> container, quint32 ignored_table);
}
END_DECLARE(Table);

class MetalTable : public Table
{
public:
    DECLARE_SAVEABLE(MetalTable, Table);
    REGISTER_CLASS_AS(MetalTable);
    MetalTable();
}
END_DECLARE(MetalTable);

}
