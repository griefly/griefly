#pragma once

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
            SYSTEM_STREAM << "Cannot find param: " << name << std::endl;
        }
        std::stringstream converter;
        converter << params_[name];
        T retval;
        converter >> retval;
        //SYSTEM_STREAM << "By key: " << name << " get param: " << retval << std::endl;
        return retval;
    }

    bool GetParamBool(std::string name)
    {
        //SYSTEM_STREAM << "By key: " << name;
        if (params_.find(name) == params_.end())
        {
            //SYSTEM_STREAM << " return false" << std::endl;
            return false;
        }
        //SYSTEM_STREAM << " return true" << std::endl;
        return true;
    }
private:
    std::map<std::string, std::string> params_; 
};

ParamsHolder& GetParamsHolder();
