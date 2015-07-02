#pragma once

#include <vector>
#include <string>

#include <SDL_ttf.h>

#include "TextInput.h"
#include "MobInt.h"

#include <QTextBrowser>

class Chat
{
public:
    static Chat* GetChat();
    static void InitChat();
    void Process();
    void DrawScroll();
    void ScrollUp();
    void ScrollDown();
    bool IsArea(int x, int y);

    void PostText(const std::string& str);
    void PostTextFor(const std::string& str, id_ptr_on<IOnMapObject> owner);
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
    int per_line_;

    std::vector<Line> lines_;

    char* text_;

    TTF_Font* deja;

    static Chat* chat;
};

void SetLogToFile();

void SetTextBrowser(QTextBrowser* tb);
QTextBrowser* GetTextBrowser();
