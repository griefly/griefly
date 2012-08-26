#include "Liquid.h"

//itemAdder<Liquid> adder;
//itemAdder<LWater> adder1;
//itemAdder<LPetrol> adder2;
//itemAdder<LHoney> adder3;
//itemAdder<LFirehoney> adder4;
//itemAdder<LBlood> adder5;

Liquid::Liquid()    
{
    SetSprite("icons/water.png");
    amount = 0;
    imageStateH = 0;
    imageStateW = 0;
    flammable = false;
}

void Liquid::processImage(SDL_Surface* s)
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
