#include "Text.h"

#include "Screen.h"

TextPainter::Text::Text(TextPainter* master, const std::string& key)
    : updater_([](std::string*){}),
    master_(master),
    how_often_(200),
    time_(0),
    key_(key)
{
}

TextPainter::Text::~Text()
{
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

void TextPainter::Text::Update()
{
    std::string new_content;
    updater_(&new_content);
    if (content_ == new_content)
    {
        return;
    }
    content_.clear();
    content_ = new_content;
    master_->AddSystemText(
        QString::fromStdString(key_),
        QString::fromStdString(content_));
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

void TextPainter::Process()
{
    for (auto it = texts_.begin(); it != texts_.end(); ++it)
    {
        if (it->second->CanUpdate())
        {
            it->second->Update();
        }
    }
}

bool TextPainter::Delete(const std::string& name)
{
    // TODO
   /* auto itr = texts_.find(name);
    if (itr == texts_.end())
    {
        return false;
    }
    
    delete itr->second;
    texts_.erase(itr);

    return true;*/
    return true;
}

TextPainter::Text& TextPainter::operator[](const std::string& name)
{
    auto value = texts_[name];
    if (value == nullptr)
    {
        texts_[name] = new Text(this, name);
    }
    return *texts_[name];
}

void TextPainter::AddSystemText(QString key, QString text)
{
    emit addSystemText(key, text);
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
