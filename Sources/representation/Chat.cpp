#include "Chat.h"

#include <sstream>

#include <SDL_ttf.h>

#include "Text.h"
#include "core/Constheader.h"
#include "Screen.h"
#include "core/objects/Creator.h"
#include "core/Helpers.h"



#include "core/VisiblePoints.h"

#include <QUuid>
#include <QTextCursor>


Chat* chat = nullptr;

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

Chat& GetChat()
{
    return *chat;
}

const size_t MAX_LINE_SIZE = 1024;
const int SCROLL_SIZE = 10;

void InitChat(QTextBrowser* tb)
{
    chat = new Chat(tb);
}

bool Chat::IsOOCMessage(const std::string &text)
{
    if (    text.length() >= 3
        && (   (text.substr(0, 3) == "OOC")
            || (text.substr(0, 3) == "ooc"))
        )
    {
        return true;
    }
    return false;
}

Chat::Chat(QTextBrowser* tb) : tb_(tb)
{

}

void Chat::SetCursorAtEnd()
{
    QTextCursor cursor = tb_->textCursor();
    cursor.movePosition(QTextCursor::End);
    tb_->setTextCursor(cursor);
}

void Chat::PostSimpleText(const std::string& str, size_t tile_id)
{
    if (!IsTileVisible(tile_id))
        return;

    SetCursorAtEnd();

    tb_->insertHtml(QString::fromStdString(str + "<br>"));

    SetCursorAtEnd();
}

void Chat::PostDamage(const std::string& by, const std::string& who, const std::string& object, size_t tile_id)
{
    if (!IsTileVisible(tile_id))
        return;

    SetCursorAtEnd();

    QString q_by = QString::fromStdString(by).toHtmlEscaped();
    QString q_who = QString::fromStdString(who).toHtmlEscaped();
    QString q_object = QString::fromStdString(object).toHtmlEscaped();

    tb_->insertHtml
        (
           "<font color=\"red\">"
         + q_who
         + " is attacked by "
         + q_by
         + " with "
         + q_object
         + "</font><br>");

    SetCursorAtEnd();
}

void Chat::PostWords(const std::string& who, const std::string& text, size_t tile_id)
{
    if (!IsTileVisible(tile_id))
        return;

    SetCursorAtEnd();

    QString q_who = QString::fromStdString(who).toHtmlEscaped();
    QString q_text = QString::fromStdString(text).toHtmlEscaped();

    tb_->insertHtml
        (
           "<b>"
         + q_who
         + "</b>: "
         + "<span>"
         + q_text
         + "</span><br>");

    SetCursorAtEnd();
}

void Chat::PostTextFor(const std::string& str, id_ptr_on<IOnMapObject> owner)
{
    if (GetMob() == owner)
        PostText(str);
}

void Chat::PostText(const std::string& str_)
{
    SetCursorAtEnd();

    QString loc = QString::fromStdString(str_).toHtmlEscaped();
    tb_->insertHtml(loc.replace('\n', "<br>"));

    SetCursorAtEnd();
}

void Chat::PostOOCText(const std::string &who, const std::string& text)
{
    SetCursorAtEnd();

    QString q_who = QString::fromStdString(who).toHtmlEscaped();
    QString q_text = QString::fromStdString(text).toHtmlEscaped();

    tb_->insertHtml
        (
           "<font color=\"blue\"><b>"
         + q_who
         + "</b>: "
         + "<span>"
         + q_text
         + "</span></font><br>");

    SetCursorAtEnd();
}

QTextBrowser* text_browser = nullptr;
void SetTextBrowser(QTextBrowser* tb)
{
    text_browser = tb;
}

QTextBrowser& GetTextBrowser()
{
    return *text_browser;
}
