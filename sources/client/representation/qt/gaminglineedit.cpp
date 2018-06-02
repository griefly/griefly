#include "representation/qt/gaminglineedit.h"

GamingLineEdit::GamingLineEdit(QWidget* parent)
    : QLineEdit(parent)
{
    is_arrow_pass_ = true;
}

void GamingLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (is_arrow_pass_)
    {
        if (   ((event && event->key() == Qt::Key_8) && (event->modifiers() == Qt::KeypadModifier))
            || ((event && event->key() == Qt::Key_2) && (event->modifiers() == Qt::KeypadModifier))
            || ((event && event->key() == Qt::Key_6) && (event->modifiers() == Qt::KeypadModifier))
            || ((event && event->key() == Qt::Key_4) && (event->modifiers() == Qt::KeypadModifier))
            || ((event && event->key() == Qt::Key_Up))
            || ((event && event->key() == Qt::Key_Down))
            || ((event && event->key() == Qt::Key_Right))
            || ((event && event->key() == Qt::Key_Left))
            )
        {
            emit keyToPass(event);
            return;
        }
    }
    if (event->key() == Qt::Key_F2)
    {
        ToggleOOC();
        return;
    }
    QLineEdit::keyPressEvent(event);
}
void GamingLineEdit::ToggleOOC(){
    QString current_text = text();
    if(IsOOCMessage(current_text))
    {
        if(current_text.length()>3)
        {
            setText(current_text.mid(current_text[3]==' '?4:3));
        }
        else
        {
            setText("");
        }
    }
    else
    {
        setText("OOC " + current_text);
    }
    return;
}

bool IsOOCMessage(const QString& text)
{
    if (    text.length() >= 3
        && (   (text.left(3) == "OOC")
            || (text.left(3) == "ooc"))
        )
    {
        return true;
    }
    return false;
}
