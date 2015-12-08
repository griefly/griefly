#pragma once

#include <vector>
#include <string>

#include <SDL_ttf.h>

#include "Mob.h"

#include <QTextBrowser>

class Chat
{
public:
    static bool IsOOCMessage(const std::string& text);
    Chat(QTextBrowser* tb);

    void Process();
    void DrawScroll();
    void ScrollUp();
    void ScrollDown();
    bool IsArea(int x, int y);

    void PostTextFor(const std::string& str, id_ptr_on<IOnMapObject> owner);

    void PostText(const std::string& str);

    void PostOOCText(const std::string& who, const std::string& str);

    void PostSimpleText(const std::string& str, size_t tile_id);
    void PostDamage(const std::string& by, const std::string& who, const std::string& object, size_t tile_id);
    void PostWords(const std::string& who, const std::string& text, size_t tile_id);
private:
    void SetCursorAtEnd();

    void ClearZone();

    void AddLines(const std::string& str);

    int CalculateAmount(const std::string& str, int from);

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

    TTF_Font* deja_;
    QTextBrowser* tb_;
};

void SetLogToFile();

Chat& GetChat();
void InitChat(QTextBrowser* tb);

/*void SetTextBrowser(QTextBrowser* tb);
QTextBrowser& GetTextBrowser();*/
