#pragma once

#include <string>

#include <SDL_ttf.h>

class TextInput
{
public:
    TextInput(int from_x, int from_y,
              int to_x, int to_y,
              int letter_size);

    void Process();

    void GetText(std::string* str);
    void AddLetter(const char* c);
    void PointerLeft();
    void PointerRight();
    void Clean();
private:
    int CalculateAmount();
    int CalculateSizeForPointer();
    void UpdateVisible();
    void NormalizePointer();

    std::string visible_text_;
    std::string text_;
    
    char* text_raw_;
    TTF_Font* deja_;

    int from_x_;
    int from_y_;
    int to_x_;
    int to_y_;
    int letter_size_;

    int border_size_;

    int pointer_pos_;
    int pointer_pixel_pos_;

    int text_pos_;

    int right_space_;
};
