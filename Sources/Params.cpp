#include "Params.h"

void ParamsHolder::ParseParams(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i)
    {
        std::string str = std::string(argv[i]);

        int num = str.find_first_of('=');
        std::string key;
        std::string value;
        if (num != std::string::npos)
        {
            key = str.substr(0, num);
            value = str.substr(num + 1);
        }
        else
        {
            key = str;
            value = "";
        }
        params_[key] = value;
        //SYSTEM_STREAM << "New param: key: " << key << " value: " << value << std::endl;
    }
}

ParamsHolder& GetParamsHolder()
{
    static ParamsHolder* params_holder = new ParamsHolder;
    return *params_holder;
}
