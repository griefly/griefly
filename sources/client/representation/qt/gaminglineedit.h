#pragma once

#include <QLineEdit>
#include <QWidget>
#include <QKeyEvent>

class GamingLineEdit: public QLineEdit
{
    Q_OBJECT
public:
    GamingLineEdit(QWidget* parent = 0);
    void keyPressEvent(QKeyEvent* event);
    void ToggleOOC();
signals:
    void keyToPass(QKeyEvent* event);
private:
    bool is_arrow_pass_;
};

bool IsOOCMessage(const QString& text);