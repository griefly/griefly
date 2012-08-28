#pragma once

#include <vector>

class IMainItem;
class IOnMapItem;

class ItemFabric
{
public:
    ItemFabric();
    size_t get_id() 
    { 
        return id_;
    }

    unsigned int get_hash_last()
    {
        return hash_last_;
    }

    std::vector<IMainItem*>& idTable()
    {
        return idTable_;
    }

    void Sync();
    void foreachProcess();

    static IMainItem* newVoidItem(unsigned int type);
    static IMainItem* newVoidItemSaved(unsigned int type);

    unsigned int hash_all();

    void saveMap(const char* path);
    void saveMap(std::stringstream& str);//to strstream
    void loadMap(const char* path);
    void loadMap(std::stringstream& str);
    void clearMap();

    template<typename T>
    id_ptr_on<T> newItemOnMap(unsigned int hash, int posx, int posy, size_t id_new = 0)
    {
        
        static_assert(std::tr1::is_same<IOnMapItem, T>::value || std::tr1::is_base_of<IOnMapItem, T>::value, "Error: MapMaster::newItemOnMap - type isn't derivied from IOnMapItem");
        T* item;
        item = castTo<T>(newVoidItem(hash));//TODO: FIX IT!(LOOK DOWN)
        if(item == 0)
        {
            SYSTEM_STREAM << "\nERROR! ERROR!\n";
            SDL_Delay(1000);
        }
        if(max(id_new, id_) >= idTable_.size())
        {
            SYSTEM_STREAM << "RESIZE MAIN TABLE\n";
            idTable_.resize(max(id_new, id_) * 2);
        }
        if(id_new == 0)
            id_new = id_++;
        else if(id_new >= id_)
        {
            SYSTEM_STREAM << id_new << " " << id_ << "id_ptr_on<T> newItemOn\n";
            id_ = id_new + 1;
        }
        item->id = id_new;
        idTable_[id_new] = item;
        if(IMainItem::mob)
        {
            item->x = (posx - IMainItem::mob->posx + beginMobPosX) * TITLE_SIZE;
            item->y = (posy - IMainItem::mob->posy + beginMobPosY) * TITLE_SIZE;
            //if(IOnMapItem::isVisible(posx, posy) && IMainItem::mob->isMove)
            //    switchDir(item->x, item->y, IMainItem::mob->dMove, IMainItem::mob->way); 
        }
        else
        {
            item->x = beginMobPosX * TITLE_SIZE;
            item->y = beginMobPosY * TITLE_SIZE;
        }
        item->posx = posx;
        item->posy = posy;
//        item->isMove = 0;
        item->lastMove = 0;
//        item->way = TITLE_SIZE;

        id_ptr_on<T> retval;
        id_ptr_on<IOnMapItem> pushval;
        pushval = item->id;
        retval = item->id;
        IMainItem::map->addItemOnMap(pushval);
        return retval;
    }
    
    template<typename T, typename TMaster>
    id_ptr_on<T> newItemSaved(TMaster master, unsigned int hash, size_t id_new = 0)
    {
        T* item;
        item = castTo<T>(newVoidItemSaved(hash));
        if(max(id_new, id_) >= idTable_.size()) 
            idTable_.resize(max(id_new, id_) * 2);
        if(id_new == 0)
            id_new = id_++;
        else if(id_new >= id_)
            id_ = id_new + 1;
        idTable_[id_new] = item;
        item->id = id_new;
        item->master = master;
        id_ptr_on<T> ret_val;
        ret_val = item->id;
        return ret_val;
    }
    
    template<typename T, typename TMaster>
    id_ptr_on<T> newItem(TMaster master, unsigned int hash, size_t id_new = 0)
    {
        T* item;
        item = castTo<T>(newVoidItem(hash));
        if(max(id_new, id_) >= idTable_.size()) 
            idTable_.resize(max(id_new, id_) * 2);
        if(id_new == 0)
            id_new = id_++;
        else if(id_new >= id_)
            id_ = id_new + 1;
        idTable_[id_new] = item;
        item->id = id_new;
        item->master = master;
        id_ptr_on<T> ret_val;
        ret_val = item->id;
        return ret_val;
    }
private:
    std::vector<IMainItem*> idTable_;
    unsigned int hash_last_;
    size_t id_;  
};