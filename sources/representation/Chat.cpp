#include "Chat.h"

#include "Text.h"
#include "core/Constheader.h"
#include "Screen.h"
#include "core/Helpers.h"
#include "core/Game.h"

#include <QUuid>
#include <QTextCursor>

Chat::Chat(GameInterface* game)
{
    game_ = game;
}

void Chat::PostSimpleText(const QString& str, quint32 tile_id)
{
    if (!game_->GetMap().IsTileVisible(tile_id))
    {
        return;
    }

    GetRepresentation().AddToNewFrame(
        Representation::ChatMessage{str.toHtmlEscaped()});
}

void Chat::PostHtmlText(const QString& str, quint32 tile_id)
{
    if (!game_->GetMap().IsTileVisible(tile_id))
    {
        return;
    }

    GetRepresentation().AddToNewFrame(
        Representation::ChatMessage{str});
}

void Chat::PostDamage(const QString& by, const QString& who, const QString& object, quint32 tile_id)
{
    if (!game_->GetMap().IsTileVisible(tile_id))
    {
        return;
    }

    QString q_by = by.toHtmlEscaped();
    QString q_who = who.toHtmlEscaped();
    QString q_object = object.toHtmlEscaped();

    GetRepresentation().AddToNewFrame(Representation::ChatMessage{
        QString("<font color=\"red\">%1 is attacked by %2 with %3</font>")
            .arg(q_by).arg(q_who).arg(q_object)});
}

void Chat::PostText(const QString& str)
{
    QString loc = str.toHtmlEscaped();
    loc.replace('\n', "<br>");
    GetRepresentation().AddToNewFrame(Representation::ChatMessage{loc});
}
