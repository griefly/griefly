#include "gaminglineedit.h"



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
    QLineEdit::keyPressEvent(event);
}
