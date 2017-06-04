#pragma once

#include <QString>
#include <QHash>

#include "Constheader.h"

#include "Hearer.h"

namespace kv
{

class ChatFrameInfo
{
public:
    void Reset();

    void PostPersonal(const QString& text, quint32 net_id);

    void PostVisible(const QString& text, const PosPoint& place);
    void PostHear(const Phrase& phrase, const PosPoint& place);

    const QVector<QString>& GetPersonalTexts(quint32 net_id) const;

    void AddFromVisibleToPersonal(const QVector<PosPoint>& points, quint32 net_id);

    void ApplyHear(Hearer* hearer);
private:
    QHash<quint32, QVector<QString>> personal_;
    QHash<PosPoint, QVector<QString>> visible_;
    QHash<PosPoint, QVector<Phrase>> hear_;
};

}
