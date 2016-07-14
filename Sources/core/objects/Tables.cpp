#include "Tables.h"

Table::Tables(size_t id) : IMovable(id)
{
    anchored = true;
    v_level = 1;
    CheckSurroundings(true);
    SetPassable(D_ALL, Passable::SMALL_CREATURE);
    up = 0;
    down = 0;
    left  = 0;
    right = 0;  
}

void Tables::Delete()
{
    CheckSurroundings(false);
    IOnMapObject::Delete();
}
void Tables::CheckSurroundings(bool table)
{
    if (auto table_up = GetNeighbour(D_UP)->GetItem<Tables>())
    {
        table_up->SetTable(D_DOWN, table);
        SetTable(D_UP, table);
    }
    if (auto table_down = GetNeighbour(D_DOWN)->GetItem<Tables>())
    {
        table_down->SetTable(D_UP, table);
        SetTable(D_DOWN, table);
    }
    if (auto table_up = GetNeighbour(D_LEFT)->GetItem<Tables>())
    {
        table_up->SetTable(D_RIGHT, table);
        SetTable(D_LEFT, table);
    }
    if (auto table_up = GetNeighbour(D_RIGHT)->GetItem<Tables>())
    {
        table_up->SetTable(D_LEFT, table);
        SetTable(D_RIGHT, table);
    }
}
void Tables::UpdateSprite()
{
    if (up + down + left + right == 4)
    {
    }
    else if (up + down + left + right == 3)
    {
        if (up + down + left == 3)
        {
            if(auto table_1 = GetNeighbour(D_LEFT)->GetNeighbour(D_UP)->GetItem<Tables>() && auto table_2 = GetNeighbour(D_LEFT)->GetNeighbour(D_DOWN)->GetItem<Tables>())
            {
            }
            else
            {
            }
        }
        else if (up + down + right == 3)
        {
            if(auto table_1 = GetNeighbour(D_RIGHT)->GetNeighbour(D_UP)->GetItem<Tables>() && auto table_2 = GetNeighbour(D_RIGHT)->GetNeighbour(D_DOWN)->GetItem<Tables>())
            {
            }
            else
            {
            }
        }
        else if (up + right + left == 3)
        {
            if(auto table_1 = GetNeighbour(D_UP)->GetNeighbour(D_LEFT)->GetItem<Tables>() && auto table_2 = GetNeighbour(D_UP)->GetNeighbour(D_RIGHT)->GetItem<Tables>())
            {
            }
            else
            {
            }
        }
        else if (right + down + left == 3)
        {
            ifauto table_1 = GetNeighbour(D_DOWN)->GetNeighbour(D_LEFT)->GetItem<Tables>() && auto table_2 = GetNeighbour(D_DOWN)->GetNeighbour(D_RIGHT)->GetItem<Tables>())
            {
            }
            else
            {
            }
        }
    }
    else if (up + down + left + right == 2)
    {
        if (up + down == 2)
        {
        }
        else if (up + left == 2)
        {
        }
        else if (up + right == 2)
        {
        }
        else if (down + left == 2)
        {
        }
        else if (down + right == 2)
        {
        }
        else if (right + left == 2)
        {
        }
    }
    else if (up + down + left + right == 1)
    {
        if (up == 1)
        {
        }
        if (down == 1)
        {
        }
        if (left == 1)
        {
        }
        if (right == 1)
        {
        }
    }
}
void Tables::SetTable(Dir directon, bool table)
{
    switch (direction)
    {
    case D_UP : up = table;
        break;  
    case D_DOWN : down = table;
        break;
    case D_LEFT : left = table;
        break;
    case D_RIGHT : right = table;
        break;
    }
    UpdateSprite();
}
void Tables::AttackBy(id_ptr_on<Item> item)
{

}
