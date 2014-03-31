#include "Testmob.h"
#include "Mob.h"

#include "MapClass.h"

//itemAdder<CTestmob> adder; 

void CTestmob::process()
{
    IOnMapItem::process();
    /*if((MAIN_TICK - lastMove > tickSpeed) && (get_rand() % 10 == 1))
    {
          //dMove = (Dir)(get_rand() % 4);
          checkMove((Dir)(get_rand() % 4));
       /// checkMove(D_UP);
    }//*/
};

CTestmob::CTestmob()
{
    passable = false;
    transparent = false;
    v_level = 5;
    SetSprite("icons/rock.png");
    name = "Rock";
};