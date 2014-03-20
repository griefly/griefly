#pragma once

#include <vector>

#include "OnMapBaseInt.h"
#include "OnMapInt.h"

template<class T>
std::iostream& operator<<(std::iostream& file, std::vector<id_ptr_on<T>>& content)
{
    file << " " << content.size() << " ";
    for (auto it = content.begin(); it != content.end(); ++it)
        file << " " << it->ret_id() << " ";
    return file;
}

template<class T>
std::iostream& operator>>(std::iostream& file, std::vector<id_ptr_on<T>>& content)
{
    size_t size;
    file >> size;
    content.reserve(size);

    unsigned int local_id;
    for (size_t i = 0; i < size; ++i)
    {
        file >> local_id;
        content.push_back(local_id);
    }
    return file;
}

class CubeTile: public IOnMapBase
{
public:
    DECLARE_SAVED(CubeTile, IOnMapBase);
    DECLARE_GET_TYPE_ITEM(CubeTile);
    CubeTile();
    virtual bool AddItem(id_ptr_on<IOnMapBase> item) override;
    virtual bool RemoveItem(id_ptr_on<IOnMapBase> item) override;
    virtual id_ptr_on<IOnMapBase> GetNeighbour(Dir direct) override;
    virtual void ForEach(std::function<void(id_ptr_on<IOnMapBase>)> callback) override;
    virtual bool IsVisibleByPlayer() const override
    {
        // TODO
        return true;       
    };
    virtual bool IsPassable() const override;
    virtual bool IsTransparent() const override;
    void SetPos(int posx, int posy, int posz = 0)
    {
        posx_ = posx;
        posy_ = posy;
        posz_ = posz;
    }
    int posx()
    {
        return posx_;
    }
    int posy()
    {
        return posy_;
    }
    int posz()
    {
        return posz_;
    }
    virtual int GetDrawX() const override
    {
        return posx_ * 32;
    }

    virtual int GetDrawY() const override
    {
        return posy_ * 32;
    }
protected:
    virtual size_t GetItemImpl(unsigned int hash) override;
private:
    int KV_SAVEBLE(posx_);
    int KV_SAVEBLE(posy_);
    int KV_SAVEBLE(posz_);
    typedef std::vector<id_ptr_on<IOnMapItem>> InsideType;
    InsideType KV_SAVEBLE(inside_list_);
    KV_ON_LOAD_CALL(LoadInMap);
    void LoadInMap(); // TODO
};
ADD_TO_TYPELIST(CubeTile);