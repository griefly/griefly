#include "Tables.h"
#include "Item.h"
#include "Human.h"

Tables::Tables(size_t id) : IMovable(id)
{
    anchored = true;
    v_level = 6;
    up_ = 0;
    down_ = 0;
    left_  = 0;
    right_ = 0; 
    SetPassable(D_ALL, Passable::SMALL_CREATURE);
    SetSprite(""); 
    material_ = "";
    SetState(material_ + "_table_d0");
}

void Tables::AfterWorldCreation()
{
    CheckSurroundings(true);
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
        table_up->SetTable(2, table);
        table_up->UpdateSprite();
        SetTable(1, table);
    }
    if (auto table_down = GetNeighbour(D_DOWN)->GetItem<Tables>())
    {
        table_down->SetTable(1, table);
        table_down->UpdateSprite();
        SetTable(2, table);
    }
    if (auto table_left = GetNeighbour(D_LEFT)->GetItem<Tables>())
    {
        table_left->SetTable(4, table);
        table_left->UpdateSprite();
        SetTable(3, table);
    }
    if (auto table_right = GetNeighbour(D_RIGHT)->GetItem<Tables>())
    {
        table_right->SetTable(3, table);
        table_right->UpdateSprite();
        SetTable(4, table);
    }
    if (auto table_upleft = GetNeighbour(D_LEFT)->GetNeighbour(D_UP)->GetItem<Tables>())
    {
        table_upleft->SetTable(8, table);
        table_upleft->UpdateSprite();
        SetTable(5, table);
    }
    if (auto table_downleft = GetNeighbour(D_LEFT)->GetNeighbour(D_DOWN)->GetItem<Tables>())
    {
        table_downleft->SetTable(7, table);
        table_downleft->UpdateSprite();
        SetTable(6, table);
    }
    if (auto table_upright = GetNeighbour(D_UP)->GetNeighbour(D_RIGHT)->GetItem<Tables>())
    {
        table_upright->SetTable(6, table);
        table_upright->UpdateSprite();
        SetTable(7, table);
    }
    if (auto table_downright = GetNeighbour(D_DOWN)->GetNeighbour(D_RIGHT)->GetItem<Tables>())
    {
        table_downright->SetTable(5, table);
        table_downright->UpdateSprite();
        SetTable(8, table);
    }
    UpdateSprite();
}
void Tables::UpdateSprite()
{
    if (up_ + down_ + left_ + right_ == 4)
    {
            SetState(material_ + "_table_d4");
    }
    else if (up_ + down_ + left_ + right_ == 3)
    {
        if (up_ + down_ + left_ == 3)
        {
            if (upleft_ && downleft_)
            {
            }
            else if (!upleft_ && downleft_)
            {
            }
            else if (upleft_ && !downleft_)
            {
            }
            else
            {
            }
        }
        else if (up_ + down_ + right_ == 3)
        {
            if (upright_ && downright_)
            {
            }
            else if (!upright_ && downright_)
            {
            }
            else if (upright_ && !downright_)
            {
            }
            else
            {
            }
        }
        else if (up_ + right_ + left_ == 3)
        {
            if (upleft_ && upright_)
            {
            }
            else if (!upleft_ && upright_)
            {
            }
            else if (upleft_ && !upright_)
            {
            }
            else
            {
            }
        }
        else if (right_ + down_ + left_ == 3)
        {
            if (downleft_ && downright_)
            {
            }
            else if (!downleft_ && downright_)
            {
            }
            else if (downleft_ && !downleft_)
            {
            }
            else
            {
            }
        }
    }
    else if (up_ + down_ + left_ + right_ == 2)
    {
        if (up_ + down_ == 2)
        {
            SetState(material_ + "_table_d2");
            Rotate(D_UP);
        }
        else if (up_ + left_ == 2)
        {
            if (upleft_)
            {
                SetState(material_ + "_table_d2_c_r");
                Rotate(D_UP);
            }
            else
            {
                SetState(material_ + "_table_d2_c_f");
                Rotate(D_UP);
            }
        }
        else if (up_ + right_ == 2)
        {
            if (upright_)
            {
                SetState(material_ + "_table_d2_c_r");
                Rotate(D_RIGHT);
            }
            else
            {
                SetState(material_ + "_table_d2_c_f");
                Rotate(D_RIGHT);
            }
        }
        else if (down_ + left_ == 2)
        {
            if (downleft_)
            {
                SetState(material_ + "_table_d2_c_r");
                Rotate(D_DOWN);
            }
            else
            {
                SetState(material_ + "_table_d2_c_f");
                Rotate(D_DOWN);
            }
        }
        else if (down_ + right_ == 2)
        {
            if (downright_)
            {
                SetState(material_ + "_table_d2_c_r");
                Rotate(D_LEFT);
            }
            else
            {
                SetState(material_ + "_table_d2_c_f");
                Rotate(D_LEFT);
            }
        }
        else if (right_ + left_ == 2)
        {
            SetState(material_ + "_table_d2");
            Rotate(D_LEFT);
        }
    }
    else if (up_ + down_ + left_ + right_ == 1)
    {
        SetState(material_ + "_table_d1");
        if (up_ == 1)
        {
            Rotate(D_UP);
        }
        if (down_ == 1)
        {
            Rotate(D_DOWN);
        }
        if (left_ == 1)
        {
            Rotate(D_LEFT);
        }
        if (right_ == 1)
        {
            Rotate(D_RIGHT);
        }
    }
    else if (up_ + down_ + left_ + right_ == 0)
    {
        SetState(material_ + "_table_d0");
    }
}
void Tables::SetTable(int direction, bool table)
{
    switch (direction)
    {
    case 1 : up_ = table;
        break;  
    case 2 : down_ = table;
        break;
    case 3 : left_ = table;
        break;
    case 4 : right_ = table;
        break;
    case 5 : upleft_ = table;
        break;  
    case 6 : downleft_ = table;
        break;
    case 7 : upright_ = table;
        break;
    case 8 : downright_ = table;
        break;
    }
}
void Tables::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Human> human= item->GetOwner())
    {
        if(auto r_item = dynamic_cast<HumanInterface*>(human->GetInterface())->GetActiveHand().Get())
            {
                GetOwner()->AddItem(r_item);
                dynamic_cast<HumanInterface*>(human->GetInterface())->Drop();
            }
    }
}

MetalTable::MetalTable(size_t id) : Tables(id)
{
    SetSprite("icons/metaltables.dmi"); 
    material_ = "metal";
    SetState(material_ + "_table_d0");
}
