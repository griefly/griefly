#pragma once

#include "Mob.h"
#include "Interface.h"
#include "View.h"

class LoginInterface: public InterfaceBase
{
    friend std::ostream& operator<<(std::stringstream& file, LoginInterface& interf);
    friend std::istream& operator>>(std::stringstream& file, LoginInterface& interf);
public:
    void Init();

    virtual bool Click(int x, int y) override;
    virtual void HandleClick(const std::string& place) override {}
    virtual void Draw() override;
    virtual unsigned int hash() const override;
private:
    View view_;
};

std::ostream& operator<<(std::stringstream& file, LoginInterface& interf);
std::istream& operator>>(std::stringstream& file, LoginInterface& interf);

inline unsigned int hash(const LoginInterface& i)
{
    return i.hash();
}

class LoginMob: public IMob
{
public:
    DECLARE_SAVED(LoginMob, IMob);
    DECLARE_GET_TYPE_ITEM(LoginMob);
    LoginMob(size_t id);
    virtual void DeinitGUI() override;
    virtual void InitGUI() override;
    virtual void GenerateInterfaceForFrame() override;
    virtual void processGUImsg(const Message2& msg) override;

    virtual void process() override;

    virtual int GetDrawX() const override
    {
        return 0;
    }

    virtual int GetDrawY() const override
    {
        return 0;
    }

    virtual int GetX() const override
    {
        return 0;
    }

    virtual int GetY() const override
    {
        return 0;
    }

    virtual int GetZ() const override
    {
        return 0;
    }

    // virtual void processImage(DrawType type) override;

    virtual InterfaceBase* GetInterface() override { return &interface_; }

    virtual void CalculateVisible(std::list<point>* visible_list) override;
private:
    LoginInterface KV_SAVEBLE(interface_);
};
ADD_TO_TYPELIST(LoginMob);
