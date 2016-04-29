#pragma once

#include <string>
#include <vector>

class Game;
class Names
{
public:
    Names(Game* game);
    std::string GetMaleName();
private:
    void LoadNames();
    Game& GetGame();
    std::vector<std::string> male_names_;
    std::vector<std::string> last_;
    Game* game_;
};
