#include "Chat.h"

#include <sstream>

#include "Text.h"
#include "core/Constheader.h"
#include "Screen.h"
#include "core/Helpers.h"
#include "core/Game.h"

#include <QUuid>
#include <QTextCursor>

std::stringstream ss;
std::fstream loc("debug_reports//" + QUuid::createUuid().toString().toStdString() + ".txt", std::ios::trunc | std::ios::out | std::ios::in);

std::ostream* local_stream = &loc;

//std::ostream* local_stream = &ss;
//std::ostream& SYSTEM_STREAM = *local_stream;

void SetLogToFile()
{
    local_stream = &loc;
}

std::ostream& get_system_stream()
{
    return *local_stream;
}

Chat::Chat(IGame* game)
{
    game_ = game;
}

void Chat::PostSimpleText(const std::string& str, quint32 tile_id)
{
    if (!game_->GetMap().IsTileVisible(tile_id))
    {
        return;
    }

    emit insertHtmlIntoChat(QString::fromStdString(str));
}

void Chat::PostDamage(const std::string& by, const std::string& who, const std::string& object, quint32 tile_id)
{
    if (!game_->GetMap().IsTileVisible(tile_id))
    {
        return;
    }

    QString q_by = QString::fromStdString(by).toHtmlEscaped();
    QString q_who = QString::fromStdString(who).toHtmlEscaped();
    QString q_object = QString::fromStdString(object).toHtmlEscaped();

    emit insertHtmlIntoChat
        (
           "<font color=\"red\">"
         + q_who
         + " is attacked by "
         + q_by
         + " with "
         + q_object
         + "</font>");
}

void Chat::PostWords(const std::string& who, const std::string& text, quint32 tile_id)
{
    if (!game_->GetMap().IsTileVisible(tile_id))
    {
        return;
    }

    QString q_who = QString::fromStdString(who).toHtmlEscaped();
    QString q_text = QString::fromStdString(text).toHtmlEscaped();

    emit insertHtmlIntoChat
        (
           "<b>"
         + q_who
         + "</b>: "
         + "<span>"
         + q_text
         + "</span>");
}

void Chat::PostTextFor(const std::string& str, IdPtr<IOnMapBase> owner)
{
    if (game_->GetMob() == owner)
    {
        PostText(str);
    }
}

void Chat::PostText(const std::string& str_)
{
    QString loc = QString::fromStdString(str_).toHtmlEscaped();
    emit insertHtmlIntoChat(loc.replace('\n', "<br>"));
}

void Chat::PostOOCText(const std::string &who, const std::string& text)
{
    QString q_who = QString::fromStdString(who).toHtmlEscaped();
    QString q_text = QString::fromStdString(text).toHtmlEscaped();

    emit insertHtmlIntoChat
        (
           "<font color=\"blue\"><b>"
         + q_who
         + "</b>: "
         + "<span>"
         + q_text
         + "</span></font>");
}
