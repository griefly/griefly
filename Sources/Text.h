#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include <SDL.h>
#include <SDL_ttf.h>

#include "constheader.h"
#include "ApproxGLImage.h"

struct NameSize
{
    std::string name;
    unsigned int size;
};

class LessNameSize
{
public:
    bool operator() (const NameSize& left, const NameSize& right);
};

class TextPainter
{
public:
    class Text
    {
        friend class TextPainter;
    public:
        Text& SetUpdater(std::function<void(std::string*)> updater);
        Text& SetFreq(int freq);
        Text& SetPlace(int x_ul, int y_ul,
                       int x_dr = -1, int y_dr = -1);
        Text& SetSize(int size);
        Text& SetColor(int r, int g, int b);
        Text& SetFont(const std::string& name);
    private:
        Text(TextPainter* master);
        void Update();
        bool CanUpdate();

        int x_ul_, y_ul_;
        int x_dr_, y_dr_;
        
        std::string font_name_;
        SDL_Color color_;
        int size_;
        
        unsigned int how_often_; // ms
        std::function<void(std::string*)> updater_;

        std::string content_;
        ApproxGLImage* content_image_;
        unsigned int time_;

        TextPainter* master_;
    };

    TTF_Font* GetFont(const std::string& name, unsigned int size);
    void Process();
    bool Delete(const std::string& name);
    Text& operator[](const std::string& name);
private:
    std::map<std::string, Text*> texts_;
    std::map<NameSize, TTF_Font*, LessNameSize> fonts_;
};

TextPainter& GetTexts();
void SetTexts(TextPainter* text_painter);