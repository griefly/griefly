#pragma once

#include <unordered_map>
#include <vector>

#include "View2.h"
#include "ViewInfo.h"

#include <QMutex>
#include <QKeyEvent>

const char* const STOP_MUSIC = "^";

class Representation
{
public:
    Representation();

    struct Entity
    {
        Entity();

        ViewInfo view;
        size_t id;
        int pos_x;
        int pos_y;
        int vlevel;
        int dir;
    };

    struct InterfaceUnit
    {
        InterfaceUnit();
        ViewInfo view;
        std::string name;

        int pixel_x;
        int pixel_y;

        int shift;
    };

    struct Sound
    {
        Sound(const std::string& name) : name(name) {}
        std::string name;
    };

    void AddToNewFrame(const InterfaceUnit& unit);
    void AddToNewFrame(const Entity& ent);
    void AddToNewFrame(const Sound& sound);

    void SetMusicForNewFrame(const std::string& music, float volume);
    void SetCameraForFrame(int pos_x, int pos_y);

    void Swap();
    void Process();
    void Click(int x, int y);

    void HandleKeyboardDown(QKeyEvent* event);
    void HandleKeyboardUp(QKeyEvent* event);
private:
    QMutex mutex_;

    void HandleInput();

    void SynchronizeViews();
    void PerformPixelMovement();
    void Draw();
    void DrawInterface();

    int pixel_movement_tick_;

    size_t current_frame_id_;

    bool is_updated_;

    struct FrameData
    {
        std::vector<Entity> entities;
        std::vector<std::string> sounds;
        std::vector<InterfaceUnit> units;
        std::string music;
        float volume;
        int camera_pos_x;
        int camera_pos_y;
    };

    typedef FrameData DataType;

    std::string current_music_;

    DataType* current_frame_;
    DataType* new_frame_;

    DataType first_data_;
    DataType second_data_;

    struct ViewWithFrameId
    {
        View2 view;
        size_t frame_id;
    };

    std::unordered_map<size_t, ViewWithFrameId> views_;
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
};

Representation &GetRepresentation();
bool IsRepresentationValid();
void SetRepresentation(Representation* g_r);
