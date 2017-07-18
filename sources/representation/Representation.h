#pragma once

#include <unordered_map>
#include <vector>

#include "View2.h"
#include "ViewInfo.h"

#include <QMap>
#include <QMutex>
#include <QKeyEvent>
#include <QElapsedTimer>

#include "Sound.h"
#include "core/Constheader.h"

class Representation : public QObject
{
    Q_OBJECT
public:
    Representation(QObject* parent = nullptr);

    struct Entity
    {
        Entity();

        ViewInfo view;
        quint32 id;
        quint32 click_id;
        int pos_x;
        int pos_y;
        int vlevel;
        Dir dir;
    };

    struct InterfaceUnit
    {
        InterfaceUnit();
        ViewInfo view;
        QString name;

        int pixel_x;
        int pixel_y;

        int shift;
    };

    struct Sound
    {
        QString name;
    };

    struct ChatMessage
    {
        QString html;
    };

    struct TextEntry
    {
        QString tab;
        QString text;
    };

    struct Music
    {
        QString name;
        int volume;
    };

    struct Performance
    {
        QElapsedTimer timer;
        qint64 mutex_ns;
    };

    const Performance& GetPerformance() { return performance_; }
    void ResetPerformance();

    void AddToNewFrame(const InterfaceUnit& unit);
    void AddToNewFrame(const Entity& entity);
    void AddToNewFrame(const Sound& sound);
    void AddToNewFrame(const ChatMessage& message);
    void AddToNewFrame(const TextEntry& text);

    void SetMusic(const Music& music);

    void SetCameraForFrame(int pos_x, int pos_y);

    void Swap();
    void Process();
    void Click(int x, int y);

    void HandleKeyboardDown(QKeyEvent* event);
    void HandleKeyboardUp(QKeyEvent* event);
    void ResetKeysState();
    void HandleInput();

    quint32 GetUniqueIdForNewFrame(quint32 base_id, quint32 number);

    SoundPlayer& GetSoundPlayer() { return player_; }
signals:
    void chatMessage(const QString& html);
    void systemText(const QString& tab, const QString& text);
    void clearSystemTexts();
    void removeEmptyTabs();
private:
    QMap<Qt::Key, bool> keys_state_;

    Performance performance_;

    QElapsedTimer autoplay_timer_;
    bool autoplay_;

    QElapsedTimer message_sending_interval_;

    QMutex mutex_;

    void SynchronizeViews();
    void PerformPixelMovement();
    void Draw();
    void DrawInterface();

    QElapsedTimer pixel_movement_tick_;

    quint32 current_frame_id_;

    bool is_updated_;

    struct FrameData
    {
        QVector<Entity> entities;
        QVector<QString> sounds;
        QVector<InterfaceUnit> units;
        QVector<ChatMessage> messages;
        QVector<TextEntry> texts;
        Music music;
        int camera_pos_x;
        int camera_pos_y;
    };

    using DataType = FrameData;

    DataType* old_frame_;
    DataType* new_frame_;

    DataType first_data_;
    DataType second_data_;

    DataType current_frame_;

    struct ViewWithFrameId
    {
        View2 view;
        quint32 frame_id;
    };

    std::unordered_map<quint32, ViewWithFrameId> views_;
    std::vector<View2> interface_views_;

    class Camera
    {
    public:
        Camera();

        void SetPos(int new_pos_x, int new_pos_y);

        void PerformPixelMovement();

        int GetFullShiftX();
        int GetFullShiftY();
    private:
        int pos_x;
        int pos_y;

        int pixel_shift_x_;
        int pixel_shift_y_;
    } camera_;

    SoundPlayer player_;
};

Representation &GetRepresentation();
bool IsRepresentationValid();
void SetRepresentation(Representation* g_r);
