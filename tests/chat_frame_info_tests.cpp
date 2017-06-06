#include "core/ChatFrameInfo.h"

#include <gtest/gtest.h>

#include <QString>

using namespace kv;

namespace
{
    void CheckInfoEmpty(const ChatFrameInfo& info)
    {
        for (quint32 net_id = 0; net_id < 256; ++net_id)
        {
            const auto& personal = info.GetPersonalTexts(net_id);
            EXPECT_EQ(personal.size(), 0);
        }
    }
}

TEST(ChatFrameInfoDeathTest, ApplyHearNullptr)
{
    EXPECT_DEATH(
    {
        ChatFrameInfo info;
        info.ApplyHear(nullptr);
    }, "hearer is nullptr");
}

TEST(ChatFrameInfo, Constructor)
{
    ChatFrameInfo info;
    CheckInfoEmpty(info);
}

TEST(ChatFrameInfo, PostPersonal)
{
    ChatFrameInfo info;

    info.PostPersonal("test", 1);
    info.PostPersonal("test2", 2);
    info.PostPersonal("test3", 3);
    info.PostPersonal("test3", 52);
    info.PostPersonal("test4", 52);

    EXPECT_EQ(info.GetPersonalTexts(0).size(), 0);

    EXPECT_EQ(info.GetPersonalTexts(1).size(), 1);
    EXPECT_TRUE(info.GetPersonalTexts(1).contains("test"));

    EXPECT_EQ(info.GetPersonalTexts(2).size(), 1);
    EXPECT_TRUE(info.GetPersonalTexts(2).contains("test2"));

    EXPECT_EQ(info.GetPersonalTexts(3).size(), 1);
    EXPECT_TRUE(info.GetPersonalTexts(3).contains("test3"));
    EXPECT_TRUE(info.GetPersonalTexts(52).contains("test3"));
    EXPECT_EQ(info.GetPersonalTexts(52).size(), 2);
    EXPECT_TRUE(info.GetPersonalTexts(52).contains("test3"));
    EXPECT_TRUE(info.GetPersonalTexts(52).contains("test4"));

    EXPECT_EQ(info.GetPersonalTexts(4).size(), 0);

    EXPECT_EQ(info.GetPersonalTexts(1321).size(), 0);

    info.Reset();
    CheckInfoEmpty(info);
}

TEST(ChatFrameInfo, PostVisible)
{
    ChatFrameInfo info;

    info.PostVisible("test", {3, 3, 3});
    info.PostVisible("test2", {3, 3, 3});
    info.PostVisible("test3", {3, 3, 3});
    info.PostVisible("test4", {1, 100, 3});
    info.PostVisible("test5", {3, 3, 10});

    qDebug() << "Post visible end";

    CheckInfoEmpty(info);

    info.AddFromVisibleToPersonal({{1, 100, 3}}, 10);
    EXPECT_EQ(info.GetPersonalTexts(10).size(), 1);
    EXPECT_TRUE(info.GetPersonalTexts(10).contains("test4"));

    info.AddFromVisibleToPersonal({{2, 30, 3}}, 17);
    EXPECT_EQ(info.GetPersonalTexts(17).size(), 0);

    info.AddFromVisibleToPersonal({{3, 3, 3}, {3, 3, 10}}, 20);
    EXPECT_EQ(info.GetPersonalTexts(20).size(), 4);
    EXPECT_TRUE(info.GetPersonalTexts(20).contains("test"));
    EXPECT_TRUE(info.GetPersonalTexts(20).contains("test2"));
    EXPECT_TRUE(info.GetPersonalTexts(20).contains("test3"));
    EXPECT_FALSE(info.GetPersonalTexts(20).contains("test4"));
    EXPECT_TRUE(info.GetPersonalTexts(20).contains("test5"));

    info.AddFromVisibleToPersonal({{3, 3, 10}}, 24);
    EXPECT_EQ(info.GetPersonalTexts(24).size(), 1);
    EXPECT_TRUE(info.GetPersonalTexts(24).contains("test5"));

    // Just in case
    EXPECT_EQ(info.GetPersonalTexts(10).size(), 1);
    EXPECT_TRUE(info.GetPersonalTexts(10).contains("test4"));

    info.Reset();
    CheckInfoEmpty(info);
}

namespace
{

class TestHearer : public Hearer
{
public:
    TestHearer(ChatFrameInfo* info, quint32 net_id, const QVector<Position>& heard_points)
        : info_(info),
          net_id_(net_id),
          heard_points_(heard_points)
    {
        // Nothing
    }

    virtual QVector<Position> GetHeardPoints() const override
    {
        return heard_points_;
    }
    virtual void Hear(const Phrase &phrase) override
    {
        info_->PostPersonal(QString("%1:%2").arg(phrase.from).arg(phrase.text), net_id_);
    }
private:
    ChatFrameInfo* info_;
    quint32 net_id_;
    QVector<Position> heard_points_;
};

}

TEST(ChatFrameInfo, PostHear)
{
    ChatFrameInfo info;

    {
        Phrase phrase;

        phrase.from = "human1";
        phrase.text = "text1";
        info.PostHear(phrase, {1, 1, 0});

        phrase.from = "human2";
        phrase.text = "text2";
        info.PostHear(phrase, {3, 10, 0});

        phrase.from = "human3";
        phrase.text = "text3";
        info.PostHear(phrase, {3, 10, 0});

        phrase.from = "human5";
        phrase.text = "text5";
        info.PostHear(phrase, {111, 111, 111});
    }

    CheckInfoEmpty(info);

    {
        TestHearer hearer(&info, 10, {{1, 1, 0}, {3, 10, 0}, {5, 5, 5}});

        info.ApplyHear(&hearer);
        EXPECT_EQ(info.GetPersonalTexts(10).size(), 3);
        EXPECT_TRUE(info.GetPersonalTexts(10).contains("human1:text1"));
        EXPECT_TRUE(info.GetPersonalTexts(10).contains("human2:text2"));
        EXPECT_TRUE(info.GetPersonalTexts(10).contains("human3:text3"));
    }
    {
        TestHearer hearer(&info, 100, {{111, 111, 111}});

        info.ApplyHear(&hearer);
        EXPECT_EQ(info.GetPersonalTexts(100).size(), 1);
        EXPECT_TRUE(info.GetPersonalTexts(100).contains("human5:text5"));

        EXPECT_EQ(info.GetPersonalTexts(10).size(), 3);
        EXPECT_TRUE(info.GetPersonalTexts(10).contains("human1:text1"));
        EXPECT_TRUE(info.GetPersonalTexts(10).contains("human2:text2"));
        EXPECT_TRUE(info.GetPersonalTexts(10).contains("human3:text3"));
    }

    info.Reset();
    CheckInfoEmpty(info);
}
