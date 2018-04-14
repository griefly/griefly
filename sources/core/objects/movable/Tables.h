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
    QString GetMaterial() const { return material_; }
    void SetMaterial(QString material) { material_ = material; }
private:
    static int CheckTable(IdPtr<MapObject> container, quint32 ignored_table);
    QString KV_SAVEABLE(material_);
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
