#include "Names.h"

#include "Game.h"
#include "SyncRandom.h"

void Names::LoadNames()
{
    {
        male_names_.clear();
        QFile male_file("names/first_male.txt");
        if (!male_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            KvAbort(QString("Unable to open %1").arg(male_file.fileName()));
        }
        while (male_file.bytesAvailable())
        {
            QString name = male_file.readLine();
            if (name.endsWith('\n'))
            {
                name = name.mid(0, name.size() - 1);
            }
            male_names_.push_back(name);
        }
    }
    {
        last_name_.clear();
        QFile last_file("names/last.txt");
        if (!last_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            KvAbort(QString("Unable to open %1").arg(last_file.fileName()));
        }
        while (last_file.bytesAvailable())
        {
            QString name = last_file.readLine();
            if (name.endsWith('\n'))
            {
                name = name.mid(0, name.size() - 1);
            }
            last_name_.push_back(name);
        }
    }
}

Names::Names(GameInterface* game)
    : game_(game)
{
    LoadNames();
}

QString Names::GetMaleName()
{
    IdPtr<kv::SynchronizedRandom> random = game_->GetGlobals()->random;

    quint32 first = random->Generate() % male_names_.size();
    quint32 last = random->Generate() % last_name_.size();
    return QString("%1 %2").arg(male_names_[first]).arg(last_name_[last]);
}
