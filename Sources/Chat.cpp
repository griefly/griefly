#include "Chat.h"

#include <sstream>

#include <SDL_ttf.h>

#include "Text.h"
#include "constheader.h"
#include "Screen.h"
#include "Creator.h"
#include "helpers.h"



#include "visible_points.h"

#include <QUuid>
#include <QTextCursor>


Chat* Chat::chat = 0;

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

Chat* Chat::GetChat()
{
    return chat;
}

const size_t MAX_LINE_SIZE = 1024;
const int SCROLL_SIZE = 10;

void Chat::InitChat()
{
    chat = new Chat;
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

void SetCursorAtEnd()
{
    QTextCursor cursor = GetTextBrowser()->textCursor();
    cursor.movePosition(QTextCursor::End);
    GetTextBrowser()->setTextCursor(cursor);
}

void Chat::PostSimpleText(const std::string& str, size_t tile_id)
{
    if (!IsTileVisible(tile_id))
        return;

    SetCursorAtEnd();

    GetTextBrowser()->insertHtml(QString::fromStdString(str + "<br>"));

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

    GetTextBrowser()->insertHtml
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

    GetTextBrowser()->insertHtml
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
    GetTextBrowser()->insertHtml(loc.replace('\n', "<br>"));

    SetCursorAtEnd();
}

void Chat::PostOOCText(const std::string &who, const std::string& text)
{
    SetCursorAtEnd();

    QString q_who = QString::fromStdString(who).toHtmlEscaped();
    QString q_text = QString::fromStdString(text).toHtmlEscaped();

    GetTextBrowser()->insertHtml
        (
           "<font color=\"blue\"><b>"
         + q_who
         + "</b>: "
         + "<span>"
         + q_text
         + "</span></font><br>");

    SetCursorAtEnd();
}

void Chat::Process()
{   
    std::string str = ss.str();
    ss.str("");

    if (str == "")
        return;
    
    PostText(str);
}

void Chat::AddLines(const std::string& str)
{
    QString loc = QString::fromStdString(str).toHtmlEscaped();
    GetTextBrowser()->append(loc);
    int pos = 0;
    while (true)
    {
        int length = CalculateAmount(str, pos);

        Line newline;
        newline.text = str.substr(pos, length);
        lines_.push_back(newline);
        ScrollDown();

        pos += length;
        if (pos == str.size() || length == 0)
            break;
    }
}

int Chat::CalculateAmount(const std::string& str, int pos)
{
    int copy_size = std::min(str.size() - pos, MAX_LINE_SIZE);
    for (int i = 0; i < copy_size; ++i)
        text_[i] = str[pos + i];

    text_[copy_size] = '\0';
    int w;
    if (int err = TTF_SizeText(deja, text_, &w, nullptr))
        std::cout << "Some ttf error " << err << std::endl;
    while (w > (to_x_ - from_x_ - SCROLL_SIZE))
    {
        --copy_size;
        text_[copy_size] = '\0';
        if (int err = TTF_SizeText(deja, text_, &w, nullptr))
            std::cout << "Some ttf error " << err << std::endl;
    }
    return copy_size;
}

void Chat::ScrollUp()
{
    if (current_pos_ < visible_lines_ * 2 - 1)
        return;
    current_pos_ -= std::min(scroll_speed_, current_pos_ - visible_lines_ * 2 + 1);
}

void Chat::ScrollDown()
{
    if (block_down_)
    {
        return;
    }
    current_pos_ += std::min(scroll_speed_, static_cast<int>(lines_.size()) - current_pos_);
}

bool Chat::IsArea(int x, int y)
{
    helpers::normalize_pixels(&x, &y);
    if (   x > from_x_ && x < to_x_
        && y > from_y_ && y < to_y_)
        return true;
    return false;
}

void Chat::ClearZone()
{
    glColor3f(0.7f, 0.7f, 0.7f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glVertex2i(from_x_, from_y_);
        glVertex2i(from_x_, to_y_);
        glVertex2i(to_x_,   to_y_);
        glVertex2i(to_x_,   from_y_);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void Chat::DrawScroll()
{
    /////
    glColor3f(0.2f, 0.2f, 0.2f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glVertex2i(to_x_ - SCROLL_SIZE, from_y_);
        glVertex2i(to_x_ - SCROLL_SIZE, to_y_ - per_line_);
        glVertex2i(to_x_,               to_y_ - per_line_);
        glVertex2i(to_x_,               from_y_);
    glEnd();
    glEnable(GL_TEXTURE_2D);


    const int POINTER_SIZE = 8;

    int pos;
    if ((static_cast<int>(lines_.size()) - (visible_lines_ * 2) + 1) <= 0)
        pos = ((to_y_ - per_line_ - POINTER_SIZE) - (from_y_ + POINTER_SIZE));
    else
        pos = (((to_y_ - per_line_ - POINTER_SIZE) - (from_y_ + POINTER_SIZE)) * (current_pos_ - (visible_lines_ * 2) + 1))
              / (lines_.size() - (visible_lines_ * 2) + 1);
    pos += POINTER_SIZE + from_y_;

    ////
    glColor3f(0.5f, 0.5f, 0.5f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glVertex2i(to_x_ - SCROLL_SIZE, pos - POINTER_SIZE);
        glVertex2i(to_x_ - SCROLL_SIZE, pos + POINTER_SIZE);
        glVertex2i(to_x_,               pos + POINTER_SIZE);
        glVertex2i(to_x_,               pos - POINTER_SIZE);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

QTextBrowser* text_browser = nullptr;
void SetTextBrowser(QTextBrowser* tb)
{
    text_browser = tb;
}

QTextBrowser* GetTextBrowser()
{
    return text_browser;
}
