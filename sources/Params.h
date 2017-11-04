#pragma once

#include <QDebug>
#include <QString>

#include <QTextStream>

#include <map>
#include <string>

#include "core/KvGlobals.h"

class ParamsHolder
{
public:
    void ParseParams(int argc, char* argv[]);

    template<class T>
    T GetParam(QString name)
    {
        if (params_.find(name) == params_.end())
        {
            qDebug() << "Cannot find param: " << name;
            return T();
        }
        QTextStream stream(&params_[name]);
        T retval = T();
        stream >> retval;
        return retval;
    }

    bool GetParamBool(QString name)
    {
        if (params_.find(name) == params_.end())
        {
            return false;
        }
        return true;
    }
private:
    std::map<QString, QString> params_; 
};

ParamsHolder& GetParamsHolder();
