#include "ChatFrameInfo.h"

using namespace kv;

void ChatFrameInfo::Reset()
{
    personal_.clear();
    visible_.clear();
    hear_.clear();
}

void ChatFrameInfo::PostPersonal(const QString& text, quint32 net_id)
{
    personal_[net_id].append(text);
}

void ChatFrameInfo::PostVisible(const QString& text, const Position& place)
{
    visible_[place].append(text);
}

void ChatFrameInfo::PostHear(const Phrase& phrase, const Position& place)
{
    hear_[place].append(phrase);
}

const QHash<Position, QVector<QString>>& ChatFrameInfo::GetVisible() const
{
    return visible_;
}

const QVector<QString>& ChatFrameInfo::GetPersonalTexts(const quint32 net_id) const
{
    const auto it = personal_.find(net_id);
    if (it == personal_.end())
    {
        static QVector<QString> const empty_retval;
        return empty_retval;
    }
    return it.value();
}

void ChatFrameInfo::AddFromVisibleToPersonal(
    const QVector<Position>& points, const quint32 net_id)
{
    auto& personal = personal_[net_id];
    for (const auto& point : points)
    {
        for (const auto& text : visible_[point])
        {
            personal.append(text);
        }
    }
}

void ChatFrameInfo::ApplyHear(Hearer* hearer)
{
    kv::Assert(hearer, "ChatFrameInfo::ApplyHear(): hearer is nullptr");

    const auto& points = hearer->GetHeardPoints();
    for (const auto& point : points)
    {
        const auto phrases = hear_.find(point);
        if (phrases == hear_.end())
        {
            continue;
        }
        for (const Phrase& phrase : *phrases)
        {
            hearer->Hear(phrase);
        }
    }
}
