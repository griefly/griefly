#include "Liquid.h"
#include "MapClass.h"

Liquid::Liquid()    
{
    SetSprite("icons/water.png");
    amount = 0;
    imageStateH = 0;
    imageStateW = 0;
    flammable = false;
}

void Liquid::SetSprite(const std::string& name)
{
    if(!map)
        return;
    sprite_ = map->aSpr.returnSpr(name);
    T_SPR = name;
};

void Liquid::processImage(DrawType type)
{
    SYSTEM_STREAM << "Very strange: someone try draw me\n";
}

void Liquid::process()
{
    if(amount > 0)
        imageStateW = 0;
    if(amount > 5)
        imageStateW = 1;
    if(amount > 15)
        imageStateW = 2;
    if(amount > 25)
        imageStateW = 3;
}
