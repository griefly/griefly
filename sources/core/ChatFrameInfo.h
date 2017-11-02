#pragma once

#include <QString>
#include <QHash>

#include "core_headers/KvGlobals.h"

#include "Hearer.h"

namespace kv
{

class ChatFrameInfo
{
public:
    void Reset();

    void PostPersonal(const QString& text, quint32 net_id);

    void PostVisible(const QString& text, const Position& place);
    void PostHear(const Phrase& phrase, const Position& place);

    const QVector<QString>& GetPersonalTexts(quint32 net_id) const;

    void AddFromVisibleToPersonal(const QVector<Position>& points, quint32 net_id);
    const QHash<Position, QVector<QString>>& GetVisible() const;

    void ApplyHear(Hearer* hearer);
private:
    QHash<quint32, QVector<QString>> personal_;
    QHash<Position, QVector<QString>> visible_;
    QHash<Position, QVector<Phrase>> hear_;
};

}
