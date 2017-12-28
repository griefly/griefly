#pragma once

#include <unordered_map>
#include <vector>

#include "View2.h"

#include <QMap>
#include <QMutex>
#include <QKeyEvent>
#include <QElapsedTimer>

#include "Sound.h"

#include "core/KvGlobals.h"
#include "core_headers/CoreInterface.h"

class Representation : public QObject
{
    Q_OBJECT
public:
    Representation(QObject* parent = nullptr);

    struct Performance
    {
        QElapsedTimer timer;
        qint64 mutex_ns;
    };

    const Performance& GetPerformance() { return performance_; }
    void ResetPerformance();

    kv::GrowingFrame GetGrowingFrame() const
    {
        return kv::GrowingFrame(new_frame_);
    }

    void Swap();
    void Process();
    void Click(int x, int y);

    void HandleKeyboardDown(QKeyEvent* event);
    void HandleKeyboardUp(QKeyEvent* event);
    void ResetKeysState();
    void HandleInput();

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

    using DataType = kv::FrameData;

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
