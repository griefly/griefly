#pragma once

#include <vector>
#include <SDL_ttf.h>

class Chat
{
public:
    static Chat* GetChat();
    static void InitChat(int from_x, int from_y, 
                         int   to_x, int   to_y, 
                         int visible_lines);
    void Process();
private:
    void AddLines(const std::string& str);

    int CalculateAmount(const std::string& str, int from);

    Chat() {};
    struct Line
    {
        std::string text;
    };
    int current_pos_;
    int visible_lines_;

    int from_x_;
    int from_y_;
    int to_x_;
    int to_y_;

    int font_size_;
    int symbols_per_line_;

    std::vector<Line> lines_;

    char* text_;

    TTF_Font* deja;

    static Chat* chat;
};