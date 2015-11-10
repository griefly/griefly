#include "Names.h"

#include <vector>
#include <fstream>

#include "SyncRandom.h"

std::vector<std::string> male_names;
std::vector<std::string> last;

void LoadNames()
{
    std::fstream male("names/first_male.txt");
    while (!male.eof())
    {
        std::string n;
        male >> n;
        male_names.push_back(n);
    }

    std::fstream last_text("names/last.txt");
    while (!last_text.eof())
    {
        std::string l;
        last_text >> l;
        last.push_back(l);
    }
}

std::string GetMaleName()
{
    if (male_names.size() == 0)
        LoadNames();
    unsigned int f = get_rand() % male_names.size();
    unsigned int l = get_rand() % last.size();
    return male_names[f] + " " + last[l];
}
