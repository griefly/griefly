#include "TextInput.h"

#include "Text.h"

const size_t MAX_LINE_SIZE = 1024;

TextInput::TextInput(
              int from_x, int from_y,
              int to_x,   int to_y,
              int letter_size)
{
    from_x_ = from_x;
    from_y_ = from_y;
    to_x_ = to_x;
    to_y_ = to_y;
    letter_size_ = letter_size;

    text_ = "ome text wow much science";
    visible_text_ = "S";

    text_pos_ = 0;
    pointer_pos_ = 0;

    pointer_pixel_pos_ = 0;

    border_size_ = 1;

    right_space_ = 2;

    text_raw_ = new char[MAX_LINE_SIZE + 1];

    deja_ = GetTexts().GetFont("DejaVuSans.ttf", letter_size);

    GetTexts()["TextInput"].SetUpdater([this](std::string* str)
    {
        *str = visible_text_;
    }).SetSize(letter_size)
      .SetPlace(from_x + 1, from_y)
      .SetFreq(50)
      .SetColor(0, 0, 0)
      .SetFont("DejaVuSans.ttf");
    UpdateVisible();
}

void TextInput::Process()
{
    // Border
    glColor3f(0.1f, 0.1f, 0.6f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glVertex2i(to_x_ - border_size_, from_y_);
        glVertex2i(to_x_ - border_size_, to_y_);
        glVertex2i(to_x_,     to_y_);
        glVertex2i(to_x_,     from_y_);
    glEnd();
    glEnable(GL_TEXTURE_2D);

    glColor3f(0.1f, 0.1f, 0.6f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glVertex2i(from_x_,     from_y_);
        glVertex2i(from_x_,     to_y_);
        glVertex2i(from_x_ + border_size_, to_y_);
        glVertex2i(from_x_ + border_size_, from_y_);
    glEnd();
    glEnable(GL_TEXTURE_2D);

    glColor3f(0.1f, 0.1f, 0.6f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glVertex2i(from_x_,     from_y_);
        glVertex2i(from_x_,     from_y_ + border_size_);
        glVertex2i(to_x_,       from_y_ + border_size_);
        glVertex2i(to_x_,       from_y_);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glColor3f(0.1f, 0.1f, 0.6f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glVertex2i(from_x_,     to_y_ - border_size_);
        glVertex2i(from_x_,     to_y_);
        glVertex2i(to_x_,       to_y_);
        glVertex2i(to_x_,       to_y_ - border_size_);
    glEnd();
    glEnable(GL_TEXTURE_2D);


    // pointer
    glColor3f(0.6f, 0.1f, 0.1f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glVertex2i(from_x_ + pointer_pixel_pos_, from_y_);
        glVertex2i(from_x_ + pointer_pixel_pos_, to_y_);
        glVertex2i(from_x_ + pointer_pixel_pos_ + 1, to_y_);
        glVertex2i(from_x_ + pointer_pixel_pos_ + 1, from_y_);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void TextInput::GetText(std::string* text)
{
    *text = text_;
}

void TextInput::Clean()
{
    text_ = "";
    visible_text_ = "";
    text_pos_ = 0;
    pointer_pos_ = 0;
    pointer_pixel_pos_ = 0;
}

void TextInput::AddLetter(char* c_)
{
    if (!c_)
    {
        if (text_pos_ + pointer_pos_ == 0)
            return;
        text_.erase(text_pos_ + pointer_pos_ - 1, 1);
        UpdateVisible();
        PointerLeft();
        return;
    }
    char c = c_[0];
    auto it = text_.begin();
    for (int i = 0; i < text_pos_ + pointer_pos_; i++)
        ++it;
    text_.insert(it, c);
    UpdateVisible();
    PointerRight();
}

void TextInput::UpdateVisible()
{
    int visible_size = CalculateAmount();

    visible_text_ = text_.substr(text_pos_, visible_size);
}

void TextInput::NormalizePointer()
{
    if (pointer_pos_ > visible_text_.size())
        pointer_pos_ = visible_text_.size();
    if (pointer_pos_ < 0)
        pointer_pos_ = 0;
     pointer_pixel_pos_ = CalculateSizeForPointer();
}

void TextInput::PointerRight()
{
    NormalizePointer();

    int global_pos = pointer_pos_ + text_pos_;

    if (pointer_pos_ == visible_text_.size())
    {
        while (    text_pos_ < text_.size() - 1
               && (visible_text_.size() + text_pos_ < global_pos + 1))
        {
            ++text_pos_;
            UpdateVisible();
        }      
        NormalizePointer();     
        return;
    }
    ++pointer_pos_;
    NormalizePointer();
}

void TextInput::PointerLeft()
{
    NormalizePointer();
    if (pointer_pos_ == 0)
    {
        if (text_pos_ > 0)
        {
            --text_pos_;
            UpdateVisible();
        }
        NormalizePointer();
        return;
    }
    --pointer_pos_;
    NormalizePointer();
}

int TextInput::CalculateAmount()
{
    int copy_size = std::min(text_.size() - text_pos_, MAX_LINE_SIZE);
    for (int i = 0; i < copy_size; ++i)
        text_raw_[i] = text_[text_pos_ + i];

    text_raw_[copy_size] = '\0';
    int w;
    if (int err = TTF_SizeText(deja_, text_raw_, &w, nullptr))
        std::cout << "Some ttf error " << err << std::endl;
    while (w > (to_x_ - from_x_ - right_space_))
    {
        --copy_size;
        text_raw_[copy_size] = '\0';
        if (int err = TTF_SizeText(deja_, text_raw_, &w, nullptr))
            std::cout << "Some ttf error " << err << std::endl;
    }
    return copy_size;
}

int TextInput::CalculateSizeForPointer()
{
    int copy_size = pointer_pos_;
    for (int i = 0; i < copy_size; ++i)
        text_raw_[i] = visible_text_[i];

    text_raw_[copy_size] = '\0';
    int w;
    if (int err = TTF_SizeText(deja_, text_raw_, &w, nullptr))
        std::cout << "Some ttf error " << err << std::endl;

 //   SYSTEM_STREAM << "POINTER PIXEL POS: " << w << std::endl;
    return w;
};