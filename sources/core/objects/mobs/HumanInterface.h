#pragma once

#include <QVector>

#include "objects/Object.h"
#include "ViewInfo.h"

#include "Slot.h"
#include "SaveableOperators.h"

namespace kv
{
class Human;
class Item;

namespace slot
{

// Slots
const QString LEFT_HAND = "left_hand";
const QString RIGHT_HAND = "right_hand";
const QString HEAD = "head";
const QString SUIT = "suit";
const QString UNIFORM = "uniform";
const QString FEET = "feet";

}

// When amount of atmos moles is too small
// temperature starts behave a little bit weird
const int ZERO_TEMPERATURE_PRESSURE_BORDER = 1000;

class HumanInterface : public Object
{
public:
    DECLARE_SAVEABLE(HumanInterface, Object);
    REGISTER_CLASS_AS(HumanInterface);
    HumanInterface();

    void SetOwner(IdPtr<Human> human);

    void HandleClick(const QString& name);

    bool PickItem(IdPtr<Item> item);
    void DropItem();

    IdPtr<Item> GetItemInActiveHand();

    IdPtr<Item> GetItem(const QString& slot_name) const;
    void RemoveItem(const QString& slot_name);
    bool InsertItem(const QString& slot_name, IdPtr<Item> item);

    void Represent(GrowingFrame* frame);

    void RemoveItem(IdPtr<Item> item);

    void AddOverlays(ViewInfo* view_info);

    void UpdatePulling(bool is_pulling);
    void UpdateLaying(bool is_laying);
    void UpdateHealth(int health);
    void UpdateEnvironment(int temperature, int pressure, int oxygen, int plasma);
private:
    bool InsertItem(Slot* slot, IdPtr<Item> item);
    static void RemoveItem(Slot* slot);

    Slot& GetSlot(const QString& slot_name);
    Slot& GetActiveHand();
    void ApplyActiveHandOnSlot(Slot* slot);

    void SwapHands();

    Button& GetButton(const QString& button_name);

    IdPtr<Human> KV_SAVEABLE(human_owner_);
    QVector<Slot> KV_SAVEABLE(slots_);
    bool KV_SAVEABLE(active_hand_);

    QVector<Button> KV_SAVEABLE(buttons_);
};
END_DECLARE(HumanInterface)

}

