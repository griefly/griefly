#pragma once

#include <vector>
#include <string>

#include <SDL_ttf.h>

class Chat
{
public:
    static Chat* GetChat();
    static void InitChat(int from_x, int from_y, 
                         int   to_x, int   to_y, 
                         int visible_lines);
    void Process();
    void DrawScroll();
    void ScrollUp();
    void ScrollDown();
    bool IsArea(int x, int y);
private:
    void ClearZone();

    void AddLines(const std::string& str);

    int CalculateAmount(const std::string& str, int from);

    Chat() {};
    struct Line
    {
        std::string text;
    };

    bool block_down_;

    int current_pos_;
    int visible_lines_;

    int scroll_speed_;

    int from_x_;
    int from_y_;
    int to_x_;
    int to_y_;

    int font_size_;

    std::vector<Line> lines_;

    char* text_;

    TTF_Font* deja;

    static Chat* chat;
};