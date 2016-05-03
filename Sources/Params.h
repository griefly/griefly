#pragma once

#include <QDebug>
#include <QString>

#include <map>
#include <string>
#include <sstream>

#include "core/Constheader.h"

class ParamsHolder
{
public:
    void ParseParams(int argc, char* argv[]);

    template<class T>
    T GetParam(std::string name)
    {
        if (params_.find(name) == params_.end())
        {
            qDebug() << "Cannot find param: " << QString::fromStdString(name);
            return T();
        }
        std::stringstream converter;
        converter << params_[name];
        T retval = T();
        converter >> retval;
        return retval;
    }

    bool GetParamBool(std::string name)
    {
        if (params_.find(name) == params_.end())
        {
            return false;
        }
        return true;
    }
private:
    std::map<std::string, std::string> params_; 
};

ParamsHolder& GetParamsHolder();
