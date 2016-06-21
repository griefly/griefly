#pragma once

#include "OnMapObject.h"
#include <SFML/System/Vector2.hpp>


class Projectile : public IOnMapObject
{
public:
    Projectile(size_t id);
    DECLARE_SAVED(Projectile,IOnMapObject)	
    DECLARE_GET_TYPE_ITEM(Projectile)
    void MakeMovementPattern(sf::Vector2i target);
    void ProcessMovement();
    virtual bool TryMove(Dir direct);
    bool CheckMoveTime();
    bool CheckPassable();
    bool MainMove();
    //TODO rotation
    virtual bool Rotate(Dir dir);
    Dir GetDir() const { return dMove; }
	//TODO remove std::map main loop proccess, ret_val on_id_ptr 
    virtual void Delete() override { IOnMapObject::Delete(); }
    virtual void Bump(id_ptr_on<IMovable> item) override;
    void Bump(id_ptr_on<Projectile> item);
    virtual void Represent() override;
    std::string KV_SAVEBLE(type);
    int KV_SAVEBLE(damage);
public:
    bool KV_SAVEBLE(anchored);
    int KV_SAVEBLE(lastMove);
    int KV_SAVEBLE(tickSpeed);
    int KV_SAVEBLE(pixSpeed);
private:
    Dir KV_SAVEBLE(dMove);
    sf::Vector2i target_;
    std::vector<VDir> movement_;
    int KV_SAVEBLE(current_step_);
    bool KV_SAVEBLE(reached_target);
};
ADD_TO_TYPELIST(Projectile);
