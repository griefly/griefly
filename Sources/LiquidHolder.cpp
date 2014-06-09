#include "LiquidHolder.h"
#include <fstream>
#include <map>
#include "MapClass.h"

//itemAdder<LiquidHolder> adder;

std::vector<Reaction> LiquidHolder::reactList;

LiquidHolder::LiquidHolder()
{
    size = 100;
}

bool LiquidHolder::LoadReaction()
{
    SYSTEM_STREAM << "Begin load raw reactions\n";
    std::fstream file;
    file.open(RAW_DIR + "reaction.txt");

    Reaction r;

    char r_name[128];
    while(!file.eof())
    {

        file >> r_name;
        SYSTEM_STREAM << r_name << "\n";
        r.one = hash(r_name);
        file >> r.oneamount;

        file >> r_name;
        SYSTEM_STREAM << r_name << "\n";
        r.two = hash(r_name);
        file >> r.twoamount;

        file >> r_name;
        SYSTEM_STREAM << r_name << "\n";
        r.three = hash(r_name);
        file >> r.threeamount;

        file >> r_name;
        SYSTEM_STREAM << r_name << "\n";
        r.result = hash(r_name);
        file >> r.resultamount;
        SYSTEM_STREAM << "\n";

        reactList.push_back(r);
    }
    file.close();
    SYSTEM_STREAM << "End load raw reactions\n";
    return 0;
}

unsigned int LiquidHolder::amountOf(unsigned int hash)
{
    //if(!liquids[hash])
    //SYSTEM_STREAM << "Begin amountof\n";
    if(liquids[hash].ret_id() == 0)
        return 0;
    return liquids[hash]->amount;
}

unsigned int LiquidHolder::amountOfAll()
{
    unsigned int retval = 0;
    for(auto it = liquids.begin(); it != liquids.end(); ++it)
        retval += amountOf(it->first);
    return retval;
}

unsigned int LiquidHolder::addLiquid(unsigned int hash, unsigned int amount)
{
    if(!amount) 
        return 0;
    int sum_amount = amountOfAll();
    if(sum_amount >= size)
        return 0;
    if(!liquids[hash].ret_id())
        liquids[hash] = IMainItem::fabric->newItem<Liquid>(master, hash);
    //SYSTEM_STREAM << liquids[hash].ret_id() << "\n";
    amount = std::min(static_cast<int>(amount), size - sum_amount);
    liquids[hash]->amount += amount;
    //SYSTEM_STREAM << liquids[hash].ret_id() << "\n";
    checkReaction(hash);
    return amount;
}

unsigned int LiquidHolder::removeLiquid(unsigned int hash, unsigned int amount)
{
    if(!liquids[hash].ret_id())
        return 0;
    if(liquids[hash]->amount < amount)
    {
        amount -= liquids[hash]->amount;
        liquids[hash]->amount = 0;
        return amount;
    }
    liquids[hash]->amount -= amount;
    return amount;
}

void LiquidHolder::checkReaction(unsigned int hash)
{    
    auto it = reactList.begin();
    for(/*done*/; it != reactList.end(); ++it)
        if(it->one == hash || it->two == hash || it->three == hash)
            break;
    if(reactList.end() == it) return;
    int one, two, three;
    one = amountOf(it->one);
    two = amountOf(it->two);
    three = amountOf(it->three);

    const int MAX_AMOUNT = 99999;

    if(it->oneamount) 
        one /= it->oneamount;
    else 
        one = MAX_AMOUNT;

    if(it->twoamount) 
        two /= it->twoamount;
    else 
        two = MAX_AMOUNT;

    if(it->threeamount) 
        three /= it->threeamount;
    else 
        three = MAX_AMOUNT;

    unsigned int react_size = std::min(std::min(one, two), three);

    removeLiquid(it->one, react_size * it->oneamount);
    removeLiquid(it->two, react_size * it->twoamount);
    removeLiquid(it->three, react_size * it->threeamount);

    addLiquid(it->result, react_size * it->resultamount);
}

void LiquidHolder::delThis()
{
    IMainItem::delThis();
}

void LiquidHolder::processImage(DrawType type)
{
    auto it = liquids.begin();
    while(it != liquids.end())
    {
        if(!it->second.ret_id() || it->second->amount < 1)
        {
            ++it;
            continue;
        }
        id_ptr_on<Liquid> l = it->second;
        auto locspr = l->GetSprite();
        /*place.x = master->x;
        place.y = master->y;
        
        const CSprite* locspr = l->GetSprite();
        SDL_BlitSurface(locspr->frames[l->imageStateW * locspr->numFrameH + l->imageStateH], NULL, s, &place);*/
        //TODO:
        //mobMaster->gl_screen->Draw(locspr, master->x, master->y, l->imageStateW, l->imageStateH);
        ++it;
    }
}

std::list<HashAmount> LiquidHolder::removeLiquid(unsigned int amount)
{
    unsigned int sum_amount = amountOfAll();
    std::list<HashAmount> retval;
    if(sum_amount == 0 || amount == 0)
        return retval;
    for(auto it = liquids.begin(); it != liquids.end(); ++it)
    {
        if(amountOf(it->first))
        {
            HashAmount ha;
            ha.amount = removeLiquid(it->first, (((amountOf(it->first) * 1000) / sum_amount) * amount) / 1000);
            ha.hash = it->first;
            retval.push_back(ha);
        }
    }
    return retval;
}

std::list<HashAmount> LiquidHolder::addLiquid(std::list<HashAmount> what)
{
    std::list<HashAmount> retval;
    
    HashAmount l;

    for(auto it = what.begin(); it != what.end(); ++it)
    {
        unsigned int loc;
        if((loc = addLiquid(it->hash, it->amount)) != 0/*it->amount*/)
        {
            l.amount = it->amount - loc;
            l.hash = it->hash;
            retval.push_back(l);
        }
    }
    return retval;
}

std::iostream& operator<<(std::iostream& file, std::map<unsigned int, id_ptr_on<Liquid>>& liquids)
{
    file << liquids.size() << " ";
    auto it = liquids.begin();
    while(it != liquids.end())
    {
        file << it->first << " ";
        file << it->second.ret_id() << " ";
        ++it;
    }
    return file;
}

std::iostream& operator>>(std::iostream& file, std::map<unsigned int, id_ptr_on<Liquid>>& liquids)
{
    int size;
    file >> size;

    unsigned int hash;
    int id_loc;
    id_ptr_on<Liquid> l;
    for(int i = 0; i < size; ++i)
    {
        file >> hash;
        file >> id_loc;
        l = id_loc;
        liquids[hash] = l;
    }
    return file;
}