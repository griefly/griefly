#pragma once

#include <QString>
#include <QVector>

#include "Interfaces.h"

class Names
{
public:
    Names(GameInterface* game);
    QString GetMaleName();
private:
    void LoadNames();
    std::vector<QString> male_names_;
    std::vector<QString> last_name_;
    GameInterface* game_;
};
