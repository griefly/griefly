#include <stdio.h>

#include "Debug.h"
#include "MainInt.h"

#include "Manager.h"
#include "NetClientImpl.h"

#include "ItemFabric.h"

#include "Params.h"

#include <QUuid>
#include <QDir>
#include <QTextStream>

const char* const DEFAULT_FOLDER = "Saves/";
const char* const SAVE_WITHOUT_NUMBER = "save";

const int AMOUNT = 15;

std::string unique_name = "Default";

Debug::Impl::UnsyncDebug& Debug::UnsyncDebug()
{
    static Debug::Impl::UnsyncDebug unsync_;
    return unsync_;
}

Debug::Impl::UnsyncDebug::UnsyncDebug()
{
    counter_ = 0;
    report_generated_ = false;
}

bool Debug::Impl::UnsyncDebug::IsReportNeeded()
{
    for (auto it = hash_history_.net_values.begin(); it != hash_history_.net_values.end(); ++it)
    {
        unsigned int val = *it->second.begin();
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            if (*it2 != val)
            {
                return true;
            }
        }
    }
    return false;
}

bool Debug::Impl::UnsyncDebug::IsReportGenerated()
{
    return report_generated_;
}

void Debug::Impl::UnsyncDebug::AddLocalSyncPair(unsigned int hash_value, unsigned int tick_value)
{
    HashTick val;
    val.hash = hash_value;
    val.tick = tick_value;
    hash_history_.local_values.push_back(val);
}

void Debug::Impl::UnsyncDebug::AddNetSyncPair(unsigned int hash_value, unsigned int tick_value)
{
    hash_history_.net_values[tick_value].push_back(hash_value);
}

void Debug::Impl::UnsyncDebug::ProcessDebug()
{
    static bool is_autogen_report = GetParamsHolder().GetParamBool("-autogen_reports");
    if (is_autogen_report)
    {
        if (!report_generated_)
        {
            if (IsReportNeeded())
            {
                if (MAIN_TICK % 20 == 1)
                {
                    GenerateAndSaveReport();
                    report_generated_ = true;
                }
            }
        }
    }
}

void Debug::Impl::UnsyncDebug::CheckSaves()
{
    const int TICK_SAVED = 50;

    static QUuid uuid = QUuid::createUuid();
    QString dir_path = "debug_reports//" + uuid.toString() + "saves";

    if (report_generated_)
    {
        return;
    }

    QDir().mkdir(dir_path);

    std::stringstream ss;

    GetItemFabric()->saveMap(ss, false);

    if (MAIN_TICK > TICK_SAVED)
    {
      /*  QFile(dir_path
              + "//map"
              + QString::number(MAIN_TICK - TICK_SAVED)
              + ".map").remove();*/
    }

}

void Debug::Impl::UnsyncDebug::GenerateAndSaveReport()
{
    QUuid uuid = QUuid::createUuid();
    QString dir_path = "debug_reports//" + uuid.toString();

    QDir().mkdir(dir_path);
    QFile file(dir_path + "//hash_report.txt");
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << Debug::UnsyncDebug().PrintHashInfo().c_str();
    }
    file.close();

    GetItemFabric()->saveMap((dir_path + "//map.map").toStdString().c_str());

    SYSTEM_STREAM << "Debug report saved as " << uuid.toString().toStdString() << std::endl;
}

std::string Debug::Impl::UnsyncDebug::PrintHashInfo()
{
    std::stringstream ss;

    ss << "Local hashes" << std::endl;
    for (auto it = hash_history_.local_values.begin(); it != hash_history_.local_values.end(); ++it)
    {
        ss << "Tick: " << it->tick << ", hash: " << it->hash << std::endl;
    }
    ss << "Net hashes" << std::endl;

    for (auto it = hash_history_.net_values.begin(); it != hash_history_.net_values.end(); ++it)
    {
        ss << "Tick: " << it->first << " Hashes: ";
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            ss << (*it2) << " ";
        }
        ss << std::endl;
    }

    return ss.str();
}

std::string Debug::Impl::UnsyncDebug::GetNextNameToSave(const std::string& folder)
{
    std::stringstream converter;
    converter << MAIN_TICK;
    return folder + unique_name + SAVE_WITHOUT_NUMBER + converter.str();
}

bool Debug::Impl::UnsyncDebug::Save()
{
    if (NetClient::GetNetClient()->Hash() != GetItemFabric()->get_hash_last())
        ++counter_;
    else
        --counter_;

    const unsigned int MAX_FAIL = 6;
    if (counter_ >= MAX_FAIL)
    {
        system("TestKill.bat");
    }

    std::stringstream converter;
    converter << MAIN_TICK - HASH_OFTEN * AMOUNT;
    remove((DEFAULT_FOLDER + unique_name + SAVE_WITHOUT_NUMBER + converter.str()).c_str());
    GetItemFabric()->saveMap(GetNextNameToSave(DEFAULT_FOLDER).c_str());
    return true;
}

void Debug::SetUniqueName(const std::string& name)
{
    unique_name = name;
}

std::string Debug::GetUniqueName()
{
    return unique_name;
}
