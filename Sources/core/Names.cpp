#include "Names.h"

#include <vector>
#include <fstream>

#include "Game.h"

void Names::LoadNames()
{
    male_names_.clear();
    std::fstream male("names/first_male.txt");
    while (!male.eof())
    {
        std::string n;
        male >> n;
        male_names_.push_back(n);
    }

    last_.clear();
    std::fstream last_text("names/last.txt");
    while (!last_text.eof())
    {
        std::string l;
        last_text >> l;
        last_.push_back(l);
    }
}

Game& Names::GetGame()
{
    return *game_;
}

Names::Names(Game* game)
    : game_(game)
{
    LoadNames();
}

std::string Names::GetMaleName()
{
    unsigned int f = GetGame().GetRandom().GetRand() % male_names_.size();
    unsigned int l = GetGame().GetRandom().GetRand() % last_.size();
    return male_names_[f] + " " + last_[l];
}
