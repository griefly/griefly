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

Chat* Chat::chat = 0;

std::stringstream ss;
std::fstream loc("debug_reports//" + QUuid::createUuid().toString().toStdString() + ".txt", std::ios::trunc | std::ios::out | std::ios::in);

std::ostream* local_stream = &ss;
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

    /*chat->from_x_ = from_x;
    chat->from_y_ = from_y;
    chat->to_x_   = to_x;
    chat->to_y_   = to_y;

    chat->visible_lines_ = visible_lines;

    chat->text_ = new char[MAX_LINE_SIZE + 1];

    chat->block_down_ = false;

    chat->scroll_speed_ = 3;

    int per_line = (to_y - from_y) / (visible_lines + 1);
    int font_size = (per_line * 3) / 4;
    chat->font_size_ = font_size;
    chat->per_line_ = per_line;

    chat->deja = GetTexts().GetFont("DejaVuSans.ttf", chat->font_size_);

    chat->text_input_ = new TextInput(from_x, to_y - per_line, to_x, to_y, font_size);

    for (int i = 0; i < chat->visible_lines_; ++i)
    {
        std::stringstream converter;
        converter << "text_line" << i;
        Chat::Line l;
        l.text = "";
        chat->lines_.push_back(l);
        GetTexts()[converter.str()].SetUpdater([&, i](std::string* str)
        {
            *str = chat->lines_[chat->current_pos_ - i - 1].text;
        }).SetSize(font_size).SetPlace(from_x + 1, to_y - (i + 2) * per_line)
            .SetFreq(50).SetColor(0, 0, 0).SetFont("DejaVuSans.ttf");
    }
    chat->current_pos_ = chat->visible_lines_;*/
}

void Chat::PostSimpleText(const std::string& str, size_t tile_id)
{
    if (!IsTileVisible(tile_id))
        return;
    GetTextBrowser()->insertHtml((str + "<br>").c_str());
}

void Chat::PostDamage(const std::string& by, const std::string& who, const std::string& object, size_t tile_id)
{
    if (!IsTileVisible(tile_id))
        return;

    GetTextBrowser()->insertHtml
        (("<font color=\"red\">" + who + " is attacked by " + by + " with " + object + "</font><br>").c_str());
}

void Chat::PostWords(const std::string& who, const std::string& text, size_t tile_id)
{
    if (!IsTileVisible(tile_id))
        return;

    GetTextBrowser()->insertHtml
        (("<b>" + who + "</b>: " + "<span>" + text + "</span><br>").c_str());
}

void Chat::PostTextFor(const std::string& str, id_ptr_on<IOnMapObject> owner)
{
    if (GetMob() == owner)
        PostText(str);
}

void Chat::PostText(const std::string& str_)
{
    QString loc = QString::fromUtf8(str_.c_str());
    GetTextBrowser()->append(loc);

    /*std::string str = str_;
    int pos = 0;
    int oldpos = 0;

    str = lines_.back().text + str;

    if (current_pos_ < static_cast<int>(lines_.size()))
        block_down_ = true;

    lines_.pop_back();
    while (pos != std::string::npos)
    {
        pos = str.find_first_of("\n", oldpos);
        std::string without = str.substr(oldpos,  pos == std::string::npos 
                                                ? pos : pos - oldpos);
        AddLines(without);
        oldpos = pos + 1;
    }
    block_down_ = false;*/
}

void Chat::Process()
{   
   /* ClearZone();
    DrawScroll();

    text_input_->Process();*/

    std::string str = ss.str();
    ss.str("");

    if (str == "")
        return;
    
    PostText(str);
}

void Chat::AddLines(const std::string& str)
{
    QString loc = QString::fromUtf8(str.c_str());
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
