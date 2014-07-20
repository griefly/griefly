#include "TextInput.h"

#include "Text.h"

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

    text_ = "";
    visible_text_ = "It is some weird text.";

    text_pos_ = 0;
    pointer_pos_ = 0;

    border_size_ = 1;

    GetTexts()["TextInput"].SetUpdater([this](std::string* str)
    {
        *str = visible_text_;
    }).SetSize(letter_size)
      .SetPlace(from_x + 1, from_y)
      .SetFreq(50)
      .SetColor(0, 0, 0)
      .SetFont("DejaVuSans.ttf");
}

void TextInput::Process()
{
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
}