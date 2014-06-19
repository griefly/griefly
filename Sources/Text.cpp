#include "Text.h"

#include "MainInt.h"
#include "MapClass.h"

SDL_Color c = {255, 255, 255, 0};

bool LessNameSize::operator() (const NameSize& left, const NameSize& right)
{
    if (left.size < right.size)
        return true;
    if (left.size == right.size)
        return std::less<std::string>()(left.name, right.name);
    return false;
}

TextPainter::Text& TextPainter::Text::SetUpdater(std::function<void(std::string*)> updater)
{
    updater_ = updater;
    return *this;
}

TextPainter::Text& TextPainter::Text::SetFreq(int freq)
{
    how_often_ = freq;
    return *this;
}
TextPainter::Text& TextPainter::Text::SetPlace(int x_ul, int y_ul,
                                               int x_dr, int y_dr)
{
    x_ul_ = x_ul;
    y_ul_ = y_ul;
    x_dr_ = x_dr;
    y_dr_ = y_dr;
    return *this;
}
TextPainter::Text& TextPainter::Text::SetSize(int size)
{
    size_ = size;
    return *this;
}
TextPainter::Text& TextPainter::Text::SetColor(int r, int g, int b)
{
    SDL_Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    color_ = c;
    return *this;
}
TextPainter::Text& TextPainter::Text::SetFont(const std::string& name)
{
    font_name_ = name;
    return *this;
}

TextPainter::Text::Text(TextPainter* master) 
    : updater_([](std::string*){}),
    master_(master),
    how_often_(200),
    content_("text_not_set"),
    font_name_("font.ttf"),
    x_ul_(0), y_ul_(0),
    x_dr_(-1), y_dr_(-1),
    color_(c),
    size_(10),
    content_image_(nullptr),
    time_(0) {}

void TextPainter::Text::Update()
{
    //content_.clear();
    std::string new_content;
    updater_(&new_content);
    if (content_ == new_content)
        return;
    content_ = new_content;
    auto local = TTF_RenderText_Blended(master_->GetFont(font_name_, size_), content_.c_str(), color_);
              
    if (local)
    {
        delete content_image_;
        content_image_ = new ApproxGLImage(local);
    }
}

bool TextPainter::Text::CanUpdate()
{
    if (SDL_GetTicks() - time_ > how_often_)
    {
        time_ = SDL_GetTicks();
        return true;
    }
    return false;
}
TTF_Font* TextPainter::GetFont(const std::string& name, unsigned int size)
{
    NameSize loc = {name, size};
    if (fonts_[loc] == nullptr)
    {
        TTF_Font* font = TTF_OpenFont(name.c_str(), size);
        if (font == nullptr)
            SYSTEM_STREAM << "Cannot load font " << name << " with size " << size << std::endl;
        fonts_[loc] = font;
    }
    return fonts_[loc];
}

void TextPainter::Process()
{
    for (auto it = texts_.begin(); it != texts_.end(); ++it)
    {
        if (it->second->CanUpdate())
            it->second->Update();

        auto image = it->second->content_image_;

        if (image == nullptr)
            continue;

        int x_ul = it->second->x_ul_, y_ul = it->second->y_ul_;
        int x_dr = it->second->x_dr_, y_dr = it->second->y_dr_;
        
        if (x_dr == -1)
            x_dr = x_ul + image->GetXSize();

        if (y_dr == -1)
            y_dr = y_ul + image->GetYSize();

        GetScreen()->Draw(image, 
                          x_ul, y_ul,
                          x_dr, y_dr);
    }
}

bool TextPainter::Delete(const std::string& name)
{
    auto itr = texts_.find(name);
    if (itr == texts_.end())
        return false;
    texts_.erase(itr);
    return true;
}

TextPainter::Text& TextPainter::operator[](const std::string& name)
{
    auto value = texts_[name];
    if (value == nullptr)
        texts_[name] = new Text(this);
    return *texts_[name];
}

TextPainter* text_painter_ = nullptr;
TextPainter& GetTexts()
{
    return *text_painter_;
}
void SetTexts(TextPainter* text_painter)
{
    text_painter_ = text_painter;
}