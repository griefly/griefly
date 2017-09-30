#include "CoreImplementation.h"
#include "CoreInterface.h"

#include "AutogenMetadata.h"

#include "core/objects/MaterialObject.h"
#include "core/objects/GlobalObjectsHolder.h"
#include "core/objects/mobs/Mob.h"
#include "core/Map.h"

#include "core/atmos/Atmos.h"
#include "core/ObjectFactory.h"
#include "core/Names.h"

namespace
{

kv::CoreInterface::ObjectsMetadata GenerateMetadata()
{
    kv::CoreInterface::ObjectsMetadata retval;

    for (auto it : (*GetItemsCreators()))
    {
        std::unique_ptr<kv::Object> object(it.second());
        kv::MaterialObject* material = CastTo<kv::MaterialObject>(object.get());
        if (!material)
        {
            qDebug() << QString("Type '%1' is not material object!").arg(it.first);
            continue;
        }

        kv::CoreInterface::ObjectMetadata metadata;
        metadata.name = it.first;
        metadata.default_view = *(material->GetView());

        retval.insert(metadata.name, metadata);
    }

    return retval;
}

}

namespace kv
{

WorldImplementation::WorldImplementation()
    : atmos_(new Atmosphere),
      factory_(new ObjectFactory(this)),
      names_(new Names(this)),
      loader_saver_(this)
{
    // Nothing
}

WorldImplementation::~WorldImplementation()
{
    // Nothing
}

void WorldImplementation::SaveWorld(FastSerializer* data) const
{
    loader_saver_.Save(*data);
}

// TODO: Look into #360 properly
void WorldImplementation::ProcessNextTick(const QVector<Message>& messages)
{
    RemoveStaleRepresentation();

    // TODO
    Q_UNUSED(messages)
}

void WorldImplementation::Represent(const QVector<PlayerAndFrame>& frames) const
{
    for (PlayerAndFrame player_and_frame : frames)
    {
        const quint32 player_net_id = player_and_frame.first;
        GrowingFrame* frame = player_and_frame.second;

        AppendSystemTexts(frame);

        VisiblePoints points;
        GetMob()->CalculateVisible(&points);

        GetMap().Represent(frame, points);

        if (IdPtr<Mob> mob = GetPlayerId(player_net_id))
        {
            mob->GenerateInterfaceForFrame(frame);
        }
        else
        {
            qDebug() << "Oops! No mob for such net id!" << player_net_id;
        }

        GetAtmosphere().Represent(frame);

        AppendSoundsToFrame(frame, points, player_net_id);
        AppendChatMessages(frame, points, player_net_id);

        // TODO: reset all shifts
        frame->SetCamera(GetMob()->GetPosition().x, GetMob()->GetPosition().y);
    }
}

void WorldImplementation::AppendSystemTexts(GrowingFrame* frame) const
{
    // TODO
}

void WorldImplementation::AppendSoundsToFrame(
    GrowingFrame* frame, const VisiblePoints& points, quint32 net_id) const
{
    // TODO: Sounds for frame may be hash table or some sorted vector
    // now scalability is quite bad
    for (auto it : qAsConst(sounds_for_frame_))
    {
        if (std::find(points.begin(), points.end(), it.first) != points.end())
        {
            frame->Append(FrameData::Sound{it.second});
        }
    }

    auto& musics_for_mobs = global_objects_->musics_for_mobs;

    auto music = musics_for_mobs.find(net_id);
    if (music != musics_for_mobs.end())
    {
        FrameData::Music frame_music;
        frame_music.name = music->first;
        frame_music.volume = music->second;
        frame->SetMusic(frame_music);
    }
}

void WorldImplementation::AppendChatMessages(
    GrowingFrame* frame, const VisiblePoints& points, quint32 net_id) const
{
    auto& visible = GetChatFrameInfo().GetVisible();
    for (const auto& point : points)
    {
        for (const auto& text : visible[point])
        {
            frame->Append(FrameData::ChatMessage{text});
        }
    }

    for (const auto& personal : chat_frame_info_.GetPersonalTexts(net_id))
    {
        frame->Append(FrameData::ChatMessage{personal});
    }
}

quint32 WorldImplementation::Hash() const
{
    return factory_->Hash();
}

AtmosInterface& WorldImplementation::GetAtmosphere()
{
    return *atmos_;
}

const AtmosInterface& WorldImplementation::GetAtmosphere() const
{
    return *atmos_;
}

MapInterface& WorldImplementation::GetMap()
{
    return *(global_objects_->map);
}

const MapInterface& WorldImplementation::GetMap() const
{
    return *(global_objects_->map);
}

ObjectFactoryInterface& WorldImplementation::GetFactory()
{
    return *factory_;
}

Names& WorldImplementation::GetNames()
{
    return *names_;
}

ChatFrameInfo& WorldImplementation::GetChatFrameInfo()
{
    return chat_frame_info_;
}

const ChatFrameInfo& WorldImplementation::GetChatFrameInfo() const
{
    return chat_frame_info_;
}

IdPtr<Mob> WorldImplementation::GetMob() const
{
    return current_mob_;
}

void WorldImplementation::SetMob(quint32 new_mob)
{
    current_mob_ = new_mob;
}

IdPtr<GlobalObjectsHolder> WorldImplementation::GetGlobals() const
{
    return global_objects_;
}

void WorldImplementation::SetGlobals(quint32 globals)
{
    global_objects_ = globals;
}

void WorldImplementation::SetPlayerId(quint32 net_id, quint32 real_id)
{
    global_objects_->players_table[net_id] = real_id;
}

quint32 WorldImplementation::GetPlayerId(quint32 net_id) const
{
    const auto& players_table = global_objects_->players_table;
    const auto it = players_table.find(net_id);
    if (it != players_table.end())
    {
        return it.value();
    }
    return 0;
}

quint32 WorldImplementation::GetNetId(quint32 real_id) const
{
    const auto& players_table = global_objects_->players_table;
    for (auto it = players_table.begin(); it != players_table.end(); ++it)
    {
        if (it.value() == real_id)
        {
            return it.key();
        }
    }
    return 0;
}

void WorldImplementation::AddSound(const QString& name, Position position)
{
    sounds_for_frame_.append({position, name});
}

void WorldImplementation::PlayMusic(const QString& name, int volume, quint32 mob)
{
    qDebug() << "Music playing:" << mob << name << volume;
    auto& musics_for_mobs = global_objects_->musics_for_mobs;
    musics_for_mobs[mob] = {name, volume};
}

void WorldImplementation::RemoveStaleRepresentation()
{
    sounds_for_frame_.clear();
    chat_frame_info_.Reset();
}

CoreImplementation::WorldPtr CoreImplementation::CreateWorldFromSave(const QByteArray& data, quint32 mob_id)
{
    // TODO
    return nullptr;
}
CoreImplementation::WorldPtr CoreImplementation::CreateWorldFromMapgen(const QByteArray& data)
{
    // TODO
    return nullptr;
}

const CoreImplementation::ObjectsMetadata& CoreImplementation::GetObjectsMetadata() const
{
    static const ObjectsMetadata metadata = GenerateMetadata();
    return metadata;
}

CoreInterface& GetCoreInstance()
{
    static CoreImplementation core;
    return core;
}

}
