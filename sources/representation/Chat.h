#pragma once

#include <string>

#include "core/objects/Mob.h"
#include "Interfaces.h"

#include <QTextBrowser>
#include <QObject>

class Chat : public ChatInterface
{
public:
    Chat(GameInterface* game);

    virtual void PostText(const QString& str) override;
    virtual void PostDamage(const QString& by, const QString& who, const QString& object, quint32 tile_id) override;
private:
    GameInterface* game_;
};
