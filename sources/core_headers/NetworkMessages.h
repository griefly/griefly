#pragma once

#include <QJsonObject>
#include <QString>

namespace Input
{
    const char* const MOVE_UP = "MOVE_UP";
    const char* const MOVE_DOWN = "MOVE_DOWN";
    const char* const MOVE_LEFT = "MOVE_LEFT";
    const char* const MOVE_RIGHT = "MOVE_RIGHT";

    const char* const KEY_Q = "KEY_Q";
    const char* const KEY_W = "KEY_W";
    const char* const KEY_E = "KEY_E";
    const char* const KEY_R = "KEY_R";
}

namespace Click
{
    const char* const LEFT = "lclick";
    const char* const LEFT_SHIFT = "lshclick";
    const char* const LEFT_CONTROL = "lctrlclick";
    const char* const LEFT_R = "lrclick";
}

inline bool IsKey(const QJsonObject& json, const QString& key)
{
    if (json["key"] == key)
    {
        return true;
    }
    return false;
}

inline quint32 ExtractObjId(const QJsonObject& json)
{
    return json["obj"].toInt();
}
inline QString ExtractAction(const QJsonObject& json)
{
    return json["action"].toString();
}
