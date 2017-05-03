#include "Tables.h"
#include "Item.h"
#include "Human.h"

Table::Table()
{
    anchored_ = true;
    v_level = 4;
    SetPassable(Dir::ALL, Passable::SMALL_CREATURE);
    SetSprite(""); 
    material_ = "";
    SetState(material_ + "_table_d0");
}

void Table::AfterWorldCreation()
{
    NotifyNeighborhood(true);
}

void Table::Delete()
{
    NotifyNeighborhood(false);
    IOnMapObject::Delete();
}
void Table::NotifyNeighborhood(bool is_in_existence)
{
    quint32 id = GetId();
    if (is_in_existence)
    {
        id = 0;
    }
    if (auto table_up = GetNeighbour(Dir::UP)->GetItem<Table>())
    {
        table_up->UpdateSprite(id);
    }
    if (auto table_down = GetNeighbour(Dir::DOWN)->GetItem<Table>())
    {
        table_down->UpdateSprite(id);
    }
    if (auto table_left = GetNeighbour(Dir::LEFT)->GetItem<Table>())
    {
        table_left->UpdateSprite(id);
    }
    if (auto table_right = GetNeighbour(Dir::RIGHT)->GetItem<Table>())
    {
        table_right->UpdateSprite(id);
    }
    if (auto table_upleft = GetNeighbour(Dir::LEFT)->GetNeighbour(Dir::UP)->GetItem<Table>())
    {
        table_upleft->UpdateSprite(id);
    }
    if (auto table_downleft = GetNeighbour(Dir::LEFT)->GetNeighbour(Dir::DOWN)->GetItem<Table>())
    {
        table_downleft->UpdateSprite(id);
    }
    if (auto table_upright = GetNeighbour(Dir::UP)->GetNeighbour(Dir::RIGHT)->GetItem<Table>())
    {
        table_upright->UpdateSprite(id);
    }
    if (auto table_downright = GetNeighbour(Dir::DOWN)->GetNeighbour(Dir::RIGHT)->GetItem<Table>())
    {
        table_downright->UpdateSprite(id);
    }
    UpdateSprite(id);
}
void Table::UpdateSprite(quint32 ignored_table)
{
    int up = CheckTable(GetNeighbour(Dir::UP), ignored_table);
    int down = CheckTable(GetNeighbour(Dir::DOWN), ignored_table);
    int left = CheckTable(GetNeighbour(Dir::LEFT), ignored_table);
    int right = CheckTable(GetNeighbour(Dir::RIGHT), ignored_table);
    int upright = CheckTable(GetNeighbour(Dir::UP)->GetNeighbour(Dir::RIGHT), ignored_table);
    int downright = CheckTable(GetNeighbour(Dir::DOWN)->GetNeighbour(Dir::RIGHT), ignored_table);
    int upleft = CheckTable(GetNeighbour(Dir::LEFT)->GetNeighbour(Dir::UP), ignored_table);
    int downleft = CheckTable(GetNeighbour(Dir::LEFT)->GetNeighbour(Dir::DOWN), ignored_table);
    if (up + down + left + right == 4)
    {
        SetState(material_ + "_table_d4");
    }
    else if (up + down + left + right == 3)
    {
        if (up + down + left == 3)
        {
            if (upleft && downleft)
            {
                SetState(material_ + "_table_d3");
                Rotate(Dir::LEFT);    
            }
            else if (!upleft && downleft)
            {
                SetState(material_ + "_table_d3_c_m_l");
                Rotate(Dir::RIGHT);
            }
            else if (upleft && !downleft)
            {
                SetState(material_ + "_table_d3_c_m_l");
                Rotate(Dir::LEFT);
            }
            else
            {
                SetState(material_ + "_table_d3_f");
                Rotate(Dir::LEFT);
            }
        }
        else if (up + down + right == 3)
        {
            if (upright && downright)
            {
                SetState(material_ + "_table_d3");
                Rotate(Dir::RIGHT);
            }
            else if (!upright && downright)
            {
                SetState(material_ + "_table_d3_c_m_r");
                Rotate(Dir::RIGHT);
            }
            else if (upright && !downright)
            {
                SetState(material_ + "_table_d3_c_m_r");
                Rotate(Dir::LEFT);
            }
            else
            {
                SetState(material_ + "_table_d3_f");
                Rotate(Dir::RIGHT);
            }
        }
        else if (up + right + left == 3)
        {
            if (upleft && upright)
            {
                SetState(material_ + "_table_d3");
                Rotate(Dir::UP);
            }
            else if (!upleft && upright)
            {
                SetState(material_ + "_table_d3_c_m_l");
                Rotate(Dir::UP);
            }
            else if (upleft && !upright)
            {
                SetState(material_ + "_table_d3_c_m_l");
                Rotate(Dir::DOWN);
            }
            else
            {
                SetState(material_ + "_table_d3_f");
                Rotate(Dir::UP);
            }
        }
        else if (right + down + left == 3)
        {
            if (downleft && downright)
            {
                SetState(material_ + "_table_d3");
                Rotate(Dir::DOWN);
            }
            else if (!downleft && downright)
            {
                SetState(material_ + "_table_d3_c_m_r");
                Rotate(Dir::UP);
            }
            else if (downleft && !downleft)
            {
                SetState(material_ + "_table_d3_c_m_r");
                Rotate(Dir::DOWN);
            }
            else
            {
                SetState(material_ + "_table_d3_f");
                Rotate(Dir::DOWN);
            }
        }
    }
    else if (up + down + left + right == 2)
    {
        if (up + down == 2)
        {
            SetState(material_ + "_table_d2");
            Rotate(Dir::UP);
        }
        else if (up + left == 2)
        {
            if (upleft)
            {
                SetState(material_ + "_table_d2_c_r");
                Rotate(Dir::LEFT);
            }
            else
            {
                SetState(material_ + "_table_d2_c_f");
                Rotate(Dir::LEFT);
            }
        }
        else if (up + right == 2)
        {
            if (upright)
            {
                SetState(material_ + "_table_d2_c_r");
                Rotate(Dir::RIGHT);
            }
            else
            {
                SetState(material_ + "_table_d2_c_f");
                Rotate(Dir::RIGHT);
            }
        }
        else if (down + left == 2)
        {
            if (downleft)
            {
                SetState(material_ + "_table_d2_c_r");
                Rotate(Dir::UP);
            }
            else
            {
                SetState(material_ + "_table_d2_c_f");
                Rotate(Dir::UP);
            }
        }
        else if (down + right == 2)
        {
            if (downright)
            {
                SetState(material_ + "_table_d2_c_r");
                Rotate(Dir::DOWN);
            }
            else
            {
                SetState(material_ + "_table_d2_c_f");
                Rotate(Dir::DOWN);
            }
        }
        else if (right + left == 2)
        {
            SetState(material_ + "_table_d2");
            Rotate(Dir::LEFT);
        }
    }
    else if (up + down + left + right == 1)
    {
        SetState(material_ + "_table_d1");
        if (up == 1)
        {
            Rotate(Dir::UP);
        }
        if (down == 1)
        {
            Rotate(Dir::DOWN);
        }
        if (left == 1)
        {
            Rotate(Dir::LEFT);
        }
        if (right == 1)
        {
            Rotate(Dir::RIGHT);
        }
    }
    else if (up + down + left + right == 0)
    {
        SetState(material_ + "_table_d0");
    }
}
void Table::AttackBy(IdPtr<Item> item)
{
    if (item.IsValid())
    {
        if (IdPtr<Human> human = item->GetOwner())
        {
            GetOwner()->AddItem(item);
            human->GetHumanInterface()->Drop();
            human->UpdateOverlays();
        }
    }
}
int Table::CheckTable(IdPtr<IOnMapBase> container, quint32 ignored_table)
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
    material_ = "metal";
    SetState(material_ + "_table_d0");
}
