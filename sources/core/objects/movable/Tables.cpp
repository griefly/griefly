#include "Tables.h"

#include "objects/movable/items/Item.h"
#include "objects/mobs/Human.h"

using namespace kv;

Table::Table()
{
    SetAnchored(true);
    SetVisibleLevel(visible_level::ITEM - 1);
    SetPassable(Dir::ALL, passable::SMALL_CREATURE);
    SetSprite(""); 
    SetMaterial("");
    SetState(GetMaterial() + "_table_d0");
}

void Table::AfterWorldCreation()
{
    NotifyNeighborhood(true);
}

void Table::Delete()
{
    NotifyNeighborhood(false);
    MaterialObject::Delete();
}
void Table::NotifyNeighborhood(bool is_in_existence)
{
    quint32 id = GetId();
    if (is_in_existence)
    {
        id = 0;
    }
    if (auto table_up = GetNeighbour(Dir::NORTH)->GetItem<Table>())
    {
        table_up->UpdateSprite(id);
    }
    if (auto table_down = GetNeighbour(Dir::SOUTH)->GetItem<Table>())
    {
        table_down->UpdateSprite(id);
    }
    if (auto table_left = GetNeighbour(Dir::WEST)->GetItem<Table>())
    {
        table_left->UpdateSprite(id);
    }
    if (auto table_right = GetNeighbour(Dir::EAST)->GetItem<Table>())
    {
        table_right->UpdateSprite(id);
    }
    if (auto table_upleft = GetNeighbour(Dir::WEST)->GetNeighbour(Dir::NORTH)->GetItem<Table>())
    {
        table_upleft->UpdateSprite(id);
    }
    if (auto table_downleft = GetNeighbour(Dir::WEST)->GetNeighbour(Dir::SOUTH)->GetItem<Table>())
    {
        table_downleft->UpdateSprite(id);
    }
    if (auto table_upright = GetNeighbour(Dir::NORTH)->GetNeighbour(Dir::EAST)->GetItem<Table>())
    {
        table_upright->UpdateSprite(id);
    }
    if (auto table_downright = GetNeighbour(Dir::SOUTH)->GetNeighbour(Dir::EAST)->GetItem<Table>())
    {
        table_downright->UpdateSprite(id);
    }
    UpdateSprite(id);
}
void Table::UpdateSprite(quint32 ignored_table)
{
    int up = CheckTable(GetNeighbour(Dir::NORTH), ignored_table);
    int down = CheckTable(GetNeighbour(Dir::SOUTH), ignored_table);
    int left = CheckTable(GetNeighbour(Dir::WEST), ignored_table);
    int right = CheckTable(GetNeighbour(Dir::EAST), ignored_table);
    int upright = CheckTable(GetNeighbour(Dir::NORTH)->GetNeighbour(Dir::EAST), ignored_table);
    int downright = CheckTable(GetNeighbour(Dir::SOUTH)->GetNeighbour(Dir::EAST), ignored_table);
    int upleft = CheckTable(GetNeighbour(Dir::WEST)->GetNeighbour(Dir::NORTH), ignored_table);
    int downleft = CheckTable(GetNeighbour(Dir::WEST)->GetNeighbour(Dir::SOUTH), ignored_table);
    if (up + down + left + right == 4)
    {
        SetState(GetMaterial() + "_table_d4");
    }
    else if (up + down + left + right == 3)
    {
        if (up + down + left == 3)
        {
            if (upleft && downleft)
            {
                SetState(GetMaterial() + "_table_d3");
                Rotate(Dir::WEST);    
            }
            else if (!upleft && downleft)
            {
                SetState(GetMaterial() + "_table_d3_c_m_l");
                Rotate(Dir::EAST);
            }
            else if (upleft && !downleft)
            {
                SetState(GetMaterial() + "_table_d3_c_m_l");
                Rotate(Dir::WEST);
            }
            else
            {
                SetState(GetMaterial() + "_table_d3_f");
                Rotate(Dir::WEST);
            }
        }
        else if (up + down + right == 3)
        {
            if (upright && downright)
            {
                SetState(GetMaterial() + "_table_d3");
                Rotate(Dir::EAST);
            }
            else if (!upright && downright)
            {
                SetState(GetMaterial() + "_table_d3_c_m_r");
                Rotate(Dir::EAST);
            }
            else if (upright && !downright)
            {
                SetState(GetMaterial() + "_table_d3_c_m_r");
                Rotate(Dir::WEST);
            }
            else
            {
                SetState(GetMaterial() + "_table_d3_f");
                Rotate(Dir::EAST);
            }
        }
        else if (up + right + left == 3)
        {
            if (upleft && upright)
            {
                SetState(GetMaterial() + "_table_d3");
                Rotate(Dir::NORTH);
            }
            else if (!upleft && upright)
            {
                SetState(GetMaterial() + "_table_d3_c_m_l");
                Rotate(Dir::NORTH);
            }
            else if (upleft && !upright)
            {
                SetState(GetMaterial() + "_table_d3_c_m_l");
                Rotate(Dir::SOUTH);
            }
            else
            {
                SetState(GetMaterial() + "_table_d3_f");
                Rotate(Dir::NORTH);
            }
        }
        else if (right + down + left == 3)
        {
            if (downleft && downright)
            {
                SetState(GetMaterial() + "_table_d3");
                Rotate(Dir::SOUTH);
            }
            else if (!downleft && downright)
            {
                SetState(GetMaterial() + "_table_d3_c_m_r");
                Rotate(Dir::NORTH);
            }
            else if (downleft && !downleft)
            {
                SetState(GetMaterial() + "_table_d3_c_m_r");
                Rotate(Dir::SOUTH);
            }
            else
            {
                SetState(GetMaterial() + "_table_d3_f");
                Rotate(Dir::SOUTH);
            }
        }
    }
    else if (up + down + left + right == 2)
    {
        if (up + down == 2)
        {
            SetState(GetMaterial() + "_table_d2");
            Rotate(Dir::NORTH);
        }
        else if (up + left == 2)
        {
            if (upleft)
            {
                SetState(GetMaterial() + "_table_d2_c_r");
                Rotate(Dir::WEST);
            }
            else
            {
                SetState(GetMaterial() + "_table_d2_c_f");
                Rotate(Dir::WEST);
            }
        }
        else if (up + right == 2)
        {
            if (upright)
            {
                SetState(GetMaterial() + "_table_d2_c_r");
                Rotate(Dir::EAST);
            }
            else
            {
                SetState(GetMaterial() + "_table_d2_c_f");
                Rotate(Dir::EAST);
            }
        }
        else if (down + left == 2)
        {
            if (downleft)
            {
                SetState(GetMaterial() + "_table_d2_c_r");
                Rotate(Dir::NORTH);
            }
            else
            {
                SetState(GetMaterial() + "_table_d2_c_f");
                Rotate(Dir::NORTH);
            }
        }
        else if (down + right == 2)
        {
            if (downright)
            {
                SetState(GetMaterial() + "_table_d2_c_r");
                Rotate(Dir::SOUTH);
            }
            else
            {
                SetState(GetMaterial() + "_table_d2_c_f");
                Rotate(Dir::SOUTH);
            }
        }
        else if (right + left == 2)
        {
            SetState(GetMaterial() + "_table_d2");
            Rotate(Dir::WEST);
        }
    }
    else if (up + down + left + right == 1)
    {
        SetState(GetMaterial() + "_table_d1");
        if (up == 1)
        {
            Rotate(Dir::NORTH);
        }
        if (down == 1)
        {
            Rotate(Dir::SOUTH);
        }
        if (left == 1)
        {
            Rotate(Dir::WEST);
        }
        if (right == 1)
        {
            Rotate(Dir::EAST);
        }
    }
    else if (up + down + left + right == 0)
    {
        SetState(GetMaterial() + "_table_d0");
    }
}
void Table::AttackBy(IdPtr<Item> /*item*/)
{
    // TODO: wrench
}

int Table::CheckTable(IdPtr<MapObject> container, quint32 ignored_table)
{
    IdPtr<Table> table = container->GetItem<Table>();
    if (!table.IsValid())
    {
        return 0;
    }
    if (table.Id() == ignored_table)
    {
        return 0;
    }
    return 1;
}

MetalTable::MetalTable()
{
    SetSprite("icons/metaltables.dmi"); 
    SetMaterial("metal");
    SetState(GetMaterial() + "_table_d0");
}
