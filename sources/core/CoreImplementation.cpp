#include "CoreImplementation.h"

#include <QDateTime>
#include <QElapsedTimer>

#include "core_headers/CoreInterface.h"

#include "AutogenMetadata.h"

#include "objects/MaterialObject.h"
#include "objects/GlobalObjectsHolder.h"
#include "objects/mobs/Mob.h"
#include "objects/mobs/LoginMob.h"
#include "Map.h"

#include "objects/test/UnsyncGenerator.h"
#include "objects/GlobalObjectsHolder.h"

#include "SynchronizedRandom.h"
#include "objects/PhysicsEngine.h"
#include "objects/Lobby.h"
#include "objects/SpawnPoints.h"
#include "objects/ObjectProcessor.h"

#include "atmos/Atmos.h"
#include "ObjectFactory.h"
#include "Names.h"

#include "Version.h"

std::vector<ObjectInfo>* id_ptr_id_table = nullptr;

namespace
{

kv::CoreInterface::ObjectsMetadata GenerateMetadata()
{
    InitSettersForTypes();

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
        metadata.default_view = material->GetView().GetRawData();
        metadata.turf = false;

        if (CastTo<kv::Turf>(object.get()))
        {
            metadata.turf = true;
        }

        const auto variables_it = GetVariablesForTypes().find(it.first);
        if (variables_it == GetVariablesForTypes().end())
        {
            kv::Abort(QString("Cannot find type '%1' in setters table!").arg(it.first));
        }

        const VariablesForType& variables = variables_it->second;
        for (auto variable = variables.begin(); variable != variables.end(); ++variable)
        {
            metadata.variables.append(variable->first);
        }

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
      process_messages_ns_(0),
      foreach_process_ns_(0),
      physics_process_ns_(0),
      atmos_process_ns_(0),
      deletion_process_ns_(0),
      update_visibility_ns_(0),
      frame_generation_ns_(0)
{
    // Nothing
}

WorldImplementation::~WorldImplementation()
{
    // Nothing
}

void WorldImplementation::StartTick()
{
    RemoveStaleRepresentation();

    // Next tick
    GetGlobals()->game_tick += 1;

    QElapsedTimer timer;

    timer.start();
    global_objects_->processor->RunProcess();
    foreach_process_ns_ = timer.nsecsElapsed();

    timer.start();
    const int game_tick = GetGlobals()->game_tick;
    GetAtmosphere().Process(game_tick);
    GetAtmosphere().ProcessConsequences(game_tick);
    atmos_process_ns_ = timer.nsecsElapsed();
}

void WorldImplementation::ProcessMessage(const Message& message)
{
    ProcessInputMessage(message);
}

void WorldImplementation::FinishTick()
{
    QElapsedTimer timer;

    timer.start();
    GetGlobals()->physics_engine->ProcessPhysics();
    physics_process_ns_ = timer.nsecsElapsed();

    ProcessHearers();

    timer.start();
    GetFactory().ProcessDeletion();
    deletion_process_ns_ = timer.nsecsElapsed();
}

void WorldImplementation::ProcessHearers()
{
    QVector<IdPtr<Object>>& hearers = global_objects_->hearers;
    QVector<IdPtr<Object>> deleted_hearers;

    for (IdPtr<Object>& hearer : hearers)
    {
        if (!hearer.IsValid())
        {
            deleted_hearers.append(hearer);
            continue;
        }
        chat_frame_info_.ApplyHear(hearer->ToHearer());
    }

    for (const IdPtr<Object>& deleted : qAsConst(deleted_hearers))
    {
        hearers.erase(std::find(hearers.begin(), hearers.end(), deleted));
    }
}

void WorldImplementation::RemoveStaleRepresentation()
{
    sounds_for_frame_.clear();
    chat_frame_info_.Reset();
}

namespace key
{

const QString ID("id");
const QString LOGIN("login");
const QString TEXT("text");

}

void WorldImplementation::ProcessInputMessage(const Message& message)
{
    if (message.type == message_type::NEW_CLIENT)
    {
        const int new_id = message.data.value(key::ID).toInt();

        {
            // Some sanity check
            const quint32 player_id = GetPlayerId(new_id);
            if (player_id != 0)
            {
                qDebug() << "Client under net_id" << new_id << "already exists";
                return;
            }
        }

        IdPtr<LoginMob> mob = GetFactory().CreateImpl(LoginMob::GetTypeStatic());
        SetPlayerId(new_id, mob.Id());
        mob->MindEnter();

        qDebug() << "New client: " << new_id << mob.Id();
        return;
    }
    else if (message.type == message_type::OOC_MESSAGE)
    {
        const QString login = message.data[key::LOGIN].toString();
        const QString text = message.data[key::TEXT].toString();
        PostOoc(login, text);
        return;
    }
    else if (   message.type == message_type::ORDINARY
             || message.type == message_type::MOUSE_CLICK
             || message.type == message_type::MESSAGE)
    {
        const int net_id = message.data[key::ID].toInt();
        const quint32 game_id = GetPlayerId(net_id);
        if (game_id == 0)
        {
            qDebug() << "Game id is 0";
        }

        IdPtr<Mob> game_object = game_id;
        kv::Assert(game_object.IsValid(), QString("Game object is not valid: %1").arg(net_id));
        game_object->ProcessMessage(message);
        return;
    }

    qDebug() << "ProcessInputMessage: Unknown message type:" << message.type;
}

void WorldImplementation::PostOoc(const QString& who, const QString& text)
{
    const QString escaped_who = who.toHtmlEscaped();
    const QString escaped_text = text.toHtmlEscaped();

    const QString post_text
        = QString("<font color=\"blue\"><b>%1</b>: <span>%2</span></font>")
            .arg(escaped_who)
            .arg(escaped_text);

    const auto& players = GetGlobals()->players_table;
    for (auto it = players.begin(); it != players.end(); ++it)
    {
        GetChatFrameInfo().PostPersonal(post_text, it.key());
    }
}

void WorldImplementation::Represent(const QVector<PlayerAndFrame>& frames) const
{
    for (const PlayerAndFrame& player_and_frame : frames)
    {
        const quint32 player_net_id = player_and_frame.first;
        GrowingFrame* frame = player_and_frame.second;

        AppendSystemTexts(frame);

        IdPtr<Mob> mob = GetPlayerId(player_net_id);
        if (!mob.IsValid())
        {
            qDebug() << "Oops! No mob for such net id!" << player_net_id;
            continue;
        }

        VisiblePoints points;
        mob->CalculateVisible(&points);
        GetMap().Represent(frame, points, mob);
        mob->GenerateInterfaceForFrame(frame);

        GetAtmosphere().Represent(frame);

        AppendSoundsToFrame(frame, points, player_net_id);
        AppendChatMessages(frame, points, player_net_id);

        // TODO: reset all shifts
        frame->SetCamera(mob->GetPosition().x, mob->GetPosition().y);
    }
}

qint32 WorldImplementation::GetGameTick() const
{
    return GetGlobals()->game_tick;
}

void WorldImplementation::AppendSystemTexts(GrowingFrame* frame) const
{
    auto append = [frame](QString tab, QString text, auto value)
    {
        frame->Append(FrameData::TextEntry{tab, text.arg(value)});
    };
    auto append_ns = [&append](QString text, qint64 ns)
    {
        append("Performance", text, (ns * 1.0) / 1000000.0);
    };

    append("Main", "Game tick: %1", GetGlobals()->game_tick);

    append_ns("Process objects: %1 ms", foreach_process_ns_);
    append_ns("Process physics movement: %1 ms", physics_process_ns_);
    append_ns("Process atmos: %1 ms", atmos_process_ns_);
    append_ns("Process deletion: %1 ms", deletion_process_ns_);
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

QByteArray WorldImplementation::SaveWorld() const
{
    FastSerializer serializer;
    world::Save(this, serializer);
    return QByteArray(serializer.GetData(), serializer.GetIndex());
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

const ObjectFactoryInterface& WorldImplementation::GetFactory() const
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

ObjectProcessorInterface& WorldImplementation::GetProcessor()
{
    return *global_objects_->processor;
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

void WorldImplementation::PerformUnsync()
{
    if (global_objects_->unsync_generator.IsValid())
    {
        global_objects_->unsync_generator->PerformUnsync();
    }
}

void WorldImplementation::AddSound(const QString& name, const Position position)
{
    sounds_for_frame_.append({position, name});
}

void WorldImplementation::PlayMusic(const QString& name, const int volume, const quint32 mob)
{
    qDebug() << "Music playing:" << mob << name << volume;
    auto& musics_for_mobs = global_objects_->musics_for_mobs;
    musics_for_mobs[mob] = {name, volume};
}

void WorldImplementation::PrepareToMapgen()
{
    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

    global_objects_ = GetFactory().CreateImpl(kv::GlobalObjectsHolder::GetTypeStatic());
    global_objects_->map = GetFactory().CreateImpl(kv::Map::GetTypeStatic());
    global_objects_->random = GetFactory().CreateImpl(kv::SynchronizedRandom::GetTypeStatic());
    global_objects_->physics_engine = GetFactory().CreateImpl(kv::PhysicsEngine::GetTypeStatic());
    global_objects_->processor = GetFactory().CreateImpl(kv::ObjectProcessor::GetTypeStatic());

    const quint32 seed = static_cast<quint32>(qrand());
    global_objects_->random->SetParams(seed, 0);
}

void WorldImplementation::AfterMapgen(const quint32 id, const bool unsync_generation)
{
    global_objects_->lobby = GetFactory().CreateImpl(kv::Lobby::GetTypeStatic());

    if (unsync_generation)
    {
        global_objects_->unsync_generator
            = GetFactory().CreateImpl(UnsyncGenerator::GetTypeStatic());
    }

    for (ObjectInfo info : GetFactory().GetIdTable())
    {
        if (info.object && (info.object->GetTypeIndex() == SpawnPoint::GetTypeIndexStatic()))
        {
            global_objects_->lobby->AddSpawnPoint(info.object->GetId());
        }
    }

    IdPtr<LoginMob> newmob = GetFactory().CreateImpl(LoginMob::GetTypeStatic());

    SetPlayerId(id, newmob.Id());
    newmob->MindEnter();
}

CoreImplementation::CoreImplementation()
{
    InitRealTypes();
    InitCastTable();
    InitSettersForTypes();
}

CoreImplementation::WorldPtr CoreImplementation::CreateWorldFromSave(
    const QByteArray& data)
{
    auto world = std::make_shared<WorldImplementation>();
    FastDeserializer deserializer(data.data(), data.size());
    world::Load(world.get(), deserializer);
    return world;
}

CoreInterface::WorldPtr CoreImplementation::CreateWorldFromJson(
    const QJsonObject& data, quint32 mob_id, const CoreInterface::Config& config)
{
    auto world = std::make_shared<WorldImplementation>();

    world->PrepareToMapgen();

    world::LoadFromJsonMapGen(world.get(), data);

    world->AfterMapgen(mob_id, config.unsync_generation);

    return world;
}

const CoreImplementation::ObjectsMetadata& CoreImplementation::GetObjectsMetadata() const
{
    static const ObjectsMetadata metadata = GenerateMetadata();
    return metadata;
}

const QString& CoreImplementation::GetGameVersion() const
{
    const static QString retval = kv::GetGameVersion();
    return retval;
}

const QString& CoreImplementation::GetBuildInfo() const
{
    const static QString retval = kv::GetBuildInfo();
    return retval;
}

const QString& CoreImplementation::GetQtVersion() const
{
    const static QString retval = kv::GetQtVersion();
    return retval;
}

CoreInterface& GetCoreInstance()
{
    static CoreImplementation core;
    return core;
}

}
