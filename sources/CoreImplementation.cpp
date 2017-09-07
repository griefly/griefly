#include "CoreInterface.h"

namespace kv
{

class CoreImplementation : public CoreInterface
{
public:
    virtual WorldPtr CreateWorldFromSave(const QByteArray& data) override
    {
        // TODO
        return nullptr;
    }
    virtual WorldPtr CreateWorldFromMapgen(const QByteArray& data)
    {
        // TODO
        return nullptr;
    }

    // <object name, metadata>
    virtual const QMap<QString, ObjectMetadata>& GetObjectsMetadata() const override
    {
        // TODO
        static const QMap<QString, ObjectMetadata> remove_me = {};
        return remove_me;
    }
};

CoreInterface& GetCoreInstance()
{
    static CoreImplementation core;
    return core;
}

}
