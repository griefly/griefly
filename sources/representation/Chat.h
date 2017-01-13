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
    static bool IsOOCMessage(const QString& text);
    Chat(IGame* game);

    virtual void PostTextFor(const QString& str, IdPtr<IOnMapBase> owner) override;
    virtual void PostHtmlFor(const QString& str, IdPtr<IOnMapBase> owner) override;
    virtual void PostText(const QString& str) override;
    virtual void PostOOCText(const QString& who, const QString& str) override;
    virtual void PostSimpleText(const QString& str, quint32 tile_id) override;
    virtual void PostHtmlText(const QString& str, quint32 tile_id) override;
    virtual void PostDamage(const QString& by, const QString& who, const QString& object, quint32 tile_id) override;
    virtual void PostWords(const QString& who, const QString& text, quint32 tile_id) override;
signals:
    void insertHtmlIntoChat(QString html);
private:
    IGame* game_;
};
