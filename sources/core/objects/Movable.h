#pragma once

#include "OnMapObject.h"

inline FastDeserializer& operator>>(FastDeserializer& file, VDir& vdir)
{
    file >> vdir.x;
    file >> vdir.y;
    file >> vdir.z;
    return file;
}

inline FastSerializer& operator<<(FastSerializer& file, const VDir& vdir)
{
    file << vdir.x;
    file << vdir.y;
    file << vdir.z;
    return file;
}

inline unsigned int hash(const VDir& vdir)
{
    return   vdir.x
           + vdir.y
           + vdir.z;
}

namespace kv
{

class IMovable : public IOnMapObject
{
public:
    DECLARE_SAVED(IMovable, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(IMovable);
    IMovable();
    void processMove();//for move
    virtual bool TryMove(Dir direct);
    bool CheckMoveTime();
    virtual bool CheckPassable();
    bool MainMove();

    virtual bool Rotate(Dir dir);
    Dir GetDir() const { return direction_; }

    virtual void ProcessForce();
    virtual void ApplyForce(VDir force) override;

    virtual void Delete() override { IOnMapObject::Delete(); }

    virtual void Represent() override;
    virtual void Bump(IdPtr<IMovable> item) override;
    virtual void BumpByGas(Dir dir, bool inside = false) override;
public:
    VDir KV_SAVEBLE(force_);
    bool KV_SAVEBLE(anchored_);
    int KV_SAVEBLE(last_move_);
    int KV_SAVEBLE(tick_speed_);

    KV_ON_LOAD_CALL(LoadInForceManager);
    void LoadInForceManager();
private:
    Dir KV_SAVEBLE(direction_);
};
ADD_TO_TYPELIST(IMovable);

}
class ForceManager
{
public:
    static ForceManager& Get();

    void Add(IdPtr<kv::IMovable> m);
    void Process();
    void Clear();
    unsigned int Hash();
private:
    std::vector<IdPtr<kv::IMovable>> under_force_;

    std::vector<IdPtr<kv::IMovable>> to_add_;
};
