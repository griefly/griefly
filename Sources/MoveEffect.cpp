#include "MoveEffect.h"
#include "MapClass.h"

Move::Move()
{
    way = 0;
    master = 0;
    direct = D_UP;
    back = false;
    pixel_speed = 1;
}

void Move::Init(int way_n, Dir direct_n, int pixel_speed_n, id_ptr_on<IOnMapObject> master_n, bool back_n)
{
    way = way_n;
    direct = direct_n;
    back = back_n;
    master = master_n;
    pixel_speed = pixel_speed_n;
}

void Move::start()
{
    int x = master->GetView()->GetStepX();
    int y = master->GetView()->GetStepY();
    MapMaster::switchDir(x, y, direct, way, back);
    master->GetView()->SetStepX(x);
    master->GetView()->SetStepY(y);
}

void Move::process()
{
    if (master.ret_id() == 0 || master.ret_item() == nullptr || way == 0)
    {
        Release();
        return;
    }
    int l = (pixel_speed <= way) ? (pixel_speed) : (pixel_speed - way);
    int x = master->GetView()->GetStepX();
    int y = master->GetView()->GetStepY();
    MapMaster::switchDir(x, y, direct, -l, back);
    master->GetView()->SetStepX(x);
    master->GetView()->SetStepY(y);
    way -= l;
}

void Move::end()
{
}

void Move::release()
{
    if (!(master.ret_id() == 0 || master.ret_item() == nullptr || way == 0))
    {
        int x = master->GetView()->GetStepX();
        int y = master->GetView()->GetStepY();
        MapMaster::switchDir(x, y, direct, -way, back);
        master->GetView()->SetStepX(x);
        master->GetView()->SetStepY(y);
    }
    way = 0;
    master = 0;
    direct = D_UP;
    back = false;
    pixel_speed = 1;
}