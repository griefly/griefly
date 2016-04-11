#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "core/Constheader.h"

#include <QObject>
#include <QElapsedTimer>

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

class TextPainter: public QObject
{
    Q_OBJECT
public:
    class Text
    {
        friend class TextPainter;
    public:
        Text& SetUpdater(std::function<void(std::string*)> updater);
        Text& SetFreq(int freq);
    private:
        Text(TextPainter* master, const std::string& key);
        ~Text();
        void Update();
        bool CanUpdate();
        
        unsigned int how_often_; // ms
        std::function<void(std::string*)> updater_;

        std::string key_;

        std::string content_;
        QElapsedTimer time_;

        TextPainter* master_;
    };
    void Process();
    bool Delete(const std::string& name);
    Text& operator[](const std::string& name);
    void AddSystemText(QString key, QString text);
signals:
    void addSystemText(QString key, QString text);
private:
    std::map<std::string, Text*> texts_;
};

TextPainter& GetTexts();
void SetTexts(TextPainter* text_painter);
