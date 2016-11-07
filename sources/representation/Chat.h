#pragma once

#include <string>

#include "core/objects/Mob.h"
#include "Interfaces.h"

#include <QTextBrowser>
#include <QObject>

class Chat: public QObject, public IChat
{
    Q_OBJECT
public:
    static bool IsOOCMessage(const std::string& text);
    Chat(IGame* game);

    void PostTextFor(const std::string& str, IdPtr<IOnMapBase> owner);
    void PostText(const std::string& str);
    void PostOOCText(const std::string& who, const std::string& str);
    void PostSimpleText(const std::string& str, quint32 tile_id);
    void PostDamage(const std::string& by, const std::string& who, const std::string& object, quint32 tile_id);
    void PostWords(const std::string& who, const std::string& text, quint32 tile_id);
signals:
    void insertHtmlIntoChat(QString html);
private:
    IGame* game_;
};

void SetLogToFile();
