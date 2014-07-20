#include "Chat.h"

#include <sstream>

#include <SDL_ttf.h>

#include "Text.h"
#include "constheader.h"

Chat* Chat::chat = 0;

std::stringstream ss;

std::ostream& SYSTEM_STREAM = ss;
std::fstream loc("errorfile.txt", std::ios::trunc | std::ios::out);

Chat* Chat::GetChat()
{
    return chat;
}

const int MAX_LINE_SIZE = 1024;

void Chat::InitChat(int from_x, int from_y, 
                    int   to_x, int   to_y, 
                    int visible_lines)
{
    chat = new Chat;

    chat->from_x_ = from_x;
    chat->from_y_ = from_y;
    chat->to_x_   = to_x;
    chat->to_y_   = to_y;

    chat->visible_lines_ = visible_lines;

    chat->text = new char[MAX_LINE_SIZE + 1];

    int per_line = (to_y - from_y) / visible_lines;
    int font_size = (per_line * 3) / 4;
    chat->font_size_ = font_size;
    chat->symbols_per_line_ = (to_x - from_x) / font_size;

    chat->deja = GetTexts().GetFont("DejaVuSans.ttf", chat->font_size_);

    SYSTEM_STREAM << "SYMBOLS PER LINE: " << chat->symbols_per_line_ << std::endl;

    for (int i = 0; i < chat->visible_lines_; ++i)
    {
        std::stringstream converter;
        converter << " " << i;
        Chat::Line l;
        l.text = converter.str();
        chat->lines_.push_back(l);
        GetTexts()[l.text].SetUpdater([&, i](std::string* str)
        {
            *str = chat->lines_[chat->current_pos_ - i - 1].text;
        }).SetSize(font_size).SetPlace(from_x, to_y - (i + 1) * per_line)
            .SetFreq(100).SetColor(0, 0, 0).SetFont("DejaVuSans.ttf");
    }
    chat->current_pos_ = chat->visible_lines_;
}

void Chat::Process()
{
    std::string str = ss.str();
    ss.str("");
    std::cout << str;

    if (str == "")
        return;
    
    int pos = str.find_first_of('\n');
    int oldpos = 0;
    while (pos != std::string::npos)
    {
        std::string without = str.substr(oldpos, pos - oldpos);
        AddLines(without);
        oldpos = pos + 1;
        pos = str.find_first_of('\n', oldpos);
    }
    AddLines(str.substr(oldpos));
}

void Chat::AddLines(const std::string& str)
{
    int pos = 0;
    while (true)
    {
        int length = CalculateAmount(str, pos);

        Line newline;
        newline.text = str.substr(pos, length);
        lines_.push_back(newline);
        ++current_pos_;

        pos += length;
        if (pos >= str.size() || length == 0)
            break;
    }
}

int Chat::CalculateAmount(const std::string& str, int pos)
{
    if (str.size() < pos + symbols_per_line_)
        return str.size() - pos;
    return symbols_per_line_;
}