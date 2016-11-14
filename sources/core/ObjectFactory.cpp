#include "ObjectFactory.h"

#include "KVAbort.h"

#include "objects/MainObject.h"
#include "objects/OnMapObject.h"
#include "Game.h"
#include "Map.h"
#include "SyncRandom.h"
#include "net/MagicStrings.h"
#include "net/NetworkMessagesTypes.h"
#include "AutogenMetadata.h"

#include "MapEditor.h"

ObjectFactory::ObjectFactory(IGame* game)
{
    objects_table_.resize(100);
    id_ = 1;
    is_world_generating_ = true;
    game_ = game;
}

unsigned int ObjectFactory::GetLastHash()
{
    return hash_last_;
}

std::vector<ObjectInfo>& ObjectFactory::GetIdTable()
{
    return objects_table_;
}

void ObjectFactory::UpdateProcessingItems()
{   
    if (!add_to_process_.size())
    {
        return;
    }
    for (auto it = add_to_process_.begin(); it != add_to_process_.end(); ++it)
    {
        if (!(*it).IsValid())
        {
            continue;
        }
        if ((*it)->GetFreq() == 0)
        {
            continue;
        }
        auto to_add = std::find(process_table_.begin(), process_table_.end(), *it);
        if (to_add == process_table_.end())
        {
            process_table_.push_back(*it);
        }
    }
    std::sort(process_table_.begin(), process_table_.end(),
    [](IdPtr<IMainObject> item1, IdPtr<IMainObject> item2)
    {
        return item1.Id() < item2.Id();
    });
    add_to_process_.clear();
}

void ObjectFactory::ForeachProcess()
{
    UpdateProcessingItems();

    quint32 table_size = process_table_.size();
    for (quint32 i = 0; i < table_size; ++i)
    {
        if (!(   process_table_[i].IsValid()
              && process_table_[i]->GetFreq()))
        {
            continue;
        }
        else if ((MAIN_TICK % process_table_[i]->GetFreq()) == 0)
        {
            process_table_[i]->Process();
        }
    }
}

void ObjectFactory::SaveMapHeader(FastSerializer& savefile)
{
    savefile << MAIN_TICK;
    savefile << id_;

    // Random save
    savefile << game_->GetRandom().GetSeed();
    savefile << game_->GetRandom().GetCallsCounter();

    // Save Map Size

    savefile << game_->GetMap().GetWidth();
    savefile << game_->GetMap().GetHeight();
    savefile << game_->GetMap().GetDepth();

    // Save player table
    savefile << static_cast<quint32>(players_table_.size());
    for (auto it = players_table_.begin(); it != players_table_.end(); ++it)
    {
        savefile << it->first;
        savefile << it->second;
    }
}

void ObjectFactory::LoadMapHeader(FastDeserializer& savefile)
{
    savefile >> MAIN_TICK;
    qDebug() << "MAIN_TICK: " << MAIN_TICK;

    savefile >> id_;
    qDebug() << "id_: " << id_;
    
    unsigned int new_seed;
    unsigned int new_calls_counter;
    savefile >> new_seed;
    savefile >> new_calls_counter;

    game_->GetRandom().SetRand(new_seed, new_calls_counter);

    objects_table_.resize(id_ + 1);

    // Load map size
    int x;
    int y;
    int z;

    savefile >> x;
    savefile >> y;
    savefile >> z;

    game_->GetMap().ResizeMap(x, y, z);

    // Load player table
    quint32 s;
    savefile >> s;
    for (quint32 i = 0; i < s; ++i)
    {
        quint32 first;
        savefile >> first;
        quint32 second;
        savefile >> second;

        qDebug() << first;
        qDebug() << second;
        SetPlayerId(first, second);
    }
}

const QString END_TYPE = "0~$";

void ObjectFactory::Save(FastSerializer& savefile)
{
    SaveMapHeader(savefile);

    if (objects_table_.empty())
    {
        qDebug() << "Trying to save empty world!";
        KvAbort();
    }

    auto it = ++objects_table_.begin();
    while (it != objects_table_.end())
    {
        if (it->object)
        {
            it->object->Save(savefile);
        }
        ++it;
    }

    savefile << END_TYPE;
}

void ObjectFactory::Load(FastDeserializer& savefile, quint32 real_this_mob)
{
    Clear();

    LoadMapHeader(savefile);
    int j = 0;
    while (!savefile.IsEnd())
    {
        j++;
        if (savefile.IsEnd())
        {
            qDebug() << "Error! " << j << "\n";
            KvAbort();
        }
        QString type;
        savefile >> type;

        if (type == END_TYPE)
        {
            qDebug() << "Zero id reached";
            break;
        }

        //SYSTEM_STREAM << "Line number: " << j << std::endl;

        quint32 id_loc;
        savefile >> id_loc;
        
        IMainObject* object = CreateVoid(type, id_loc);
        object->Load(savefile);
    }
    qDebug() << "\n NUM OF ELEMENTS CREATED: " << j;
    qDebug() << "SET MOB START" << GetPlayerId(real_this_mob);
    game_->SetMob(GetPlayerId(real_this_mob));
    qDebug() << "SET MOB END" << game_->GetMob().Id();
    game_->ChangeMob(game_->GetMob());
    is_world_generating_ = false;

    game_->GetMap().GetAtmosphere().LoadGrid();
}

void ObjectFactory::LoadFromMapGen(const QString& name)
{
    //qDebug() << "Start clear";
    Clear();
    //qDebug() << "End clear";

    QFile file(name);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error open " << name;
        return;
    }

    QByteArray raw_data = file.readAll();
    FastDeserializer ss(raw_data.data(), raw_data.size());

    BeginWorldCreation();

    int x;
    int y;
    int z;

    ss >> x;
    ss >> y;
    ss >> z;

    game_->MakeTiles(x, y, z);

    qDebug() << "Begin loading cycle";
    while (!ss.IsEnd())
    {
        QString item_type;
        quint32 x;
        quint32 y;
        quint32 z;

        ss >> item_type;

        ss >> x;
        ss >> y;
        ss >> z;

        //qDebug() << "Create<IOnMapObject>" << &game_->GetFactory();
        //qDebug() << "Create<IOnMapObject> " << QString::fromStdString(t_item);
        IdPtr<IOnMapObject> i = CreateImpl(item_type);
        if (!i.IsValid())
        {
            qDebug() << "Unable to cast: " << item_type;
            KvAbort();
        }
        //qDebug() << "Success!";

        MapgenVariablesType variables;
        WrapReadMessage(ss, variables);

        for (auto it = variables.begin(); it != variables.end(); ++it)
        {
            if ((it->second.size() == 0) || (it->first.size() == 0))
            {
                continue;
            }

            QByteArray variable_data = it->second;

            FastDeserializer local(variable_data.data(), variable_data.size());

            get_setters_for_types()[item_type][it->first](
                i.operator*(),
                local);
        }

        //qDebug() << "id_ptr_on<ITurf> t = i";
        if (IdPtr<ITurf> t = i)
        {
            if (game_->GetMap().GetSquares()[x][y][z]->GetTurf())
            {
                qDebug() << "DOUBLE TURF!";
            }
            game_->GetMap().GetSquares()[x][y][z]->SetTurf(t);
        }
        else
        {
            game_->GetMap().GetSquares()[x][y][z]->AddItem(i);
        }
    }
    FinishWorldCreation();
}

IMainObject* ObjectFactory::NewVoidObject(const QString& type, quint32 id)
{
    //qDebug() << "NewVoidObject: " << QString::fromStdString(type);
    auto& il = (*items_creators());
    //qDebug() << il.size();
    auto f = il[type];

    //qDebug() << f;

    IMainObject* retval = f(id);
    //qDebug() << "NewVoidObject end";
    return retval;
}

IMainObject* ObjectFactory::NewVoidObjectSaved(const QString& type)
{
    return (*items_void_creators())[type]();
}

void ObjectFactory::Clear()
{
    quint32 table_size = objects_table_.size();
    for (quint32 i = 1; i < table_size; ++i)
    {
        if (objects_table_[i].object != nullptr)
        {
            delete objects_table_[i].object;
        }
    }
    if (table_size != objects_table_.size())
    {
        qDebug() << "WARNING: table_size != idTable_.size()!";
    }

    ids_to_delete_.clear();
    process_table_.clear();
    add_to_process_.clear();
    players_table_.clear();
    add_to_process_.clear();

    id_ = 1;
}

void ObjectFactory::BeginWorldCreation()
{
    is_world_generating_ = true;
}

void ObjectFactory::FinishWorldCreation()
{
    is_world_generating_ = false;
    quint32 table_size = objects_table_.size();
    for (quint32 i = 1; i < table_size; ++i)
    {
        if (objects_table_[i].object != nullptr)
        {
            objects_table_[i].object->AfterWorldCreation();
        }
    }
}

quint32 ObjectFactory::CreateImpl(const QString &type, quint32 owner_id)
{
    IMainObject* item = NewVoidObject(type, id_);
    if (item == 0)
    {
        qDebug() << "Unable to create object: " << type;
        KvAbort();
    }
    item->SetGame(game_);

    if (id_ >= objects_table_.size())
    {
        objects_table_.resize(id_ * 2);
    }
    objects_table_[id_].object = item;
    quint32 retval = id_;
    ++id_;
    IdPtr<IOnMapBase> owner = owner_id;
    if (owner.IsValid())
    {
        if (CastTo<ITurf>(item) != nullptr)
        {
            qDebug() << "is_turf == true";
            owner->SetTurf(item->GetId());
        }
        else if (!owner->AddItem(item->GetId()))
        {
            qDebug() << "AddItem failed";
            KvAbort();
        }
    }

    if (!is_world_generating_)
    {
        item->AfterWorldCreation();
    }
    return retval;
}

IMainObject* ObjectFactory::CreateVoid(const QString &hash, quint32 id_new)
{
    IMainObject* item = NewVoidObjectSaved(hash);
    item->SetGame(game_);
    if (id_new >= objects_table_.size())
    {
        objects_table_.resize(id_new * 2);
    }

    if (id_new >= id_)
    {
        id_ = id_new + 1;
    }
    objects_table_[id_new].object = item;
    item->SetId(id_new);
    return item;
}

void ObjectFactory::DeleteLater(quint32 id)
{
    ids_to_delete_.push_back(objects_table_[id].object);
    objects_table_[id].object = nullptr;
}

void ObjectFactory::ProcessDeletion()
{
    for (auto it = ids_to_delete_.begin(); it != ids_to_delete_.end(); ++it)
    {
        delete *it;
    }
    ids_to_delete_.clear();
}

unsigned int ObjectFactory::Hash()
{
    unsigned int h = 0;
    quint32 table_size = objects_table_.size();
    for (quint32 i = 1; i < table_size; ++i)
    {
        if (objects_table_[i].object != nullptr)
        {
            h += objects_table_[i].object->Hash();
        }
    }

    ForceManager::Get().Clear();
    h += ForceManager::Get().Hash();

    ClearProcessing();

    int i = 1;
    for (auto p = process_table_.begin(); p != process_table_.end(); ++p)
    {
        h += p->Id() * i;
        i++;
    }

    return h;
}

void ObjectFactory::SetPlayerId(quint32 net_id, quint32 real_id)
{
    players_table_[net_id] = real_id;
}
quint32 ObjectFactory::GetPlayerId(quint32 net_id)
{
    auto it = players_table_.find(net_id);
    if (it != players_table_.end())
    {
        return it->second;
    }
    return 0;
}

quint32 ObjectFactory::GetNetId(quint32 real_id)
{
    for (auto it = players_table_.begin(); it != players_table_.end(); ++it)
        if (it->second == real_id)
            return it->first;
    return 0;
}

void ObjectFactory::AddProcessingItem(quint32 item)
{
    add_to_process_.push_back(item);
}

void ObjectFactory::ClearProcessing()
{
    std::vector<IdPtr<IMainObject>> remove_from_process;
    quint32 table_size = process_table_.size();
    for (quint32 i = 0; i < table_size; ++i)
    {
        if (!(   process_table_[i].IsValid()
              && process_table_[i]->GetFreq()))
        {
            remove_from_process.push_back(process_table_[i]);
        }
    }

    for (auto it = remove_from_process.begin(); it != remove_from_process.end(); ++it)
    {
        process_table_.erase(std::find(process_table_.begin(), process_table_.end(), *it));
    }
}
