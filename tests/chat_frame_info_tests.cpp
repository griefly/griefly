#include "core/ChatFrameInfo.h"

#include <gtest/gtest.h>

using namespace kv;

TEST(ChatFrameInfo, Constructor)
{
    ChatFrameInfo info;

    for (quint32 net_id = 0; net_id < 256; ++net_id)
    {
        const auto& personal = info.GetPersonalTexts(net_id);
        EXPECT_EQ(personal.size(), 0);
    }
}

