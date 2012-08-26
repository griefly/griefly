#pragma once

#include <vector>

template<typename TEffect>
TEffect* getEffectOf()
{
    static EffectFabricOf<TEffect> fabric;
    return fabric.Get();
}

class IFabric
{
public:
    virtual void process() = 0;
};

class FabricProcesser
{
public:
    static FabricProcesser* Get()
    {
        static FabricProcesser fb;
        return &fb;
    }
    void AddFabric(IFabric* fb)
    {
        fabrics_.push_back(fb);
    }
    void process()
    {
        for (int i = fabrics_.size() - 1; i >= 0; --i)
            fabrics_[i]->process();
    }
private:
    std::vector<IFabric*> fabrics_;
};

template<typename TEffect>
class EffectFabricOf : public IFabric
{
public:
    virtual void process() override
    {
        for(int i = effects_.size() - 1; i >= 0; --i)
            if (effects_[i]->getState() == TEffect::PROCESS)
                effects_[i]->process();
    }
    friend TEffect* getEffectOf<TEffect>();
private:
    EffectFabricOf()
    {
        SYSTEM_STREAM << "NewFabric!" << "\n";
        FabricProcesser::Get()->AddFabric(this);
        last_used_ = 0;
        for(int i = 0; i < TEffect::PREP_SIZE; ++i)
            effects_.push_back(new TEffect);
    }
    TEffect* Get()
    {
        int l_size = effects_.size();
        for(int i = (last_used_ + 1) % l_size; i != last_used_; i = (++i) % l_size)
            if(effects_[i]->getState() == BaseEffect::UNUSED)
            {
                last_used_ = i;
                return effects_[i];
            }
        for(int i = 0; i < l_size; ++i)
            effects_.push_back(new TEffect);
        last_used_ = l_size + 1;
        return effects_[last_used_];
    }
    int last_used_;
    std::vector<TEffect*> effects_;
};

class BaseEffect
{
public:
    //friend 
    static const int PREP_SIZE = 10;
    enum STATE {END, PROCESS, UNUSED};
    STATE getState() 
    {
        return state_;
    }
    bool isProcess() 
    {
        return state_ == PROCESS;
    }
    virtual void process() = 0;
    
    void End()
    {
        end();
        state_ = END;
    }
    void Release()
    {
        release();
        state_ = UNUSED;
    }
    void Start()
    {
        start();
        state_ = PROCESS;
    }
protected:
    virtual void release() {};
    virtual void start() = 0;
    virtual void end() = 0;
    BaseEffect()
    {
        state_ = UNUSED;
    }
    STATE state_;
};
