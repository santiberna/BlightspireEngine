#include "main_engine.hpp"
#include <gtest/gtest.h>

namespace TestModules
{

class TestModule : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override
    {
        return ModuleTickOrder::eFirst;
    };

    void Tick([[maybe_unused]] Engine& engine) override { };
    void Shutdown([[maybe_unused]] Engine& engine) override { };
};

class DependentModule : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override
    {
        engine.GetModule<TestModule>();
        return ModuleTickOrder::eFirst;
    };

    void Tick([[maybe_unused]] Engine& engine) override { };
    void Shutdown([[maybe_unused]] Engine& engine) override { };
};

class CheckUpdateModule : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override
    {
        return ModuleTickOrder::eTick;
    };

    void Tick([[maybe_unused]] Engine& engine) override
    {
        _has_updated = true;
    };

    void Shutdown([[maybe_unused]] Engine& engine) override {

    };

public:
    bool _has_updated = false;
};

class SelfDestructModuleFirst : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override
    {
        return ModuleTickOrder::eFirst;
    };

    void Tick(Engine& engine) override
    {
        engine.RequestShutdown(-1);
    };

    void Shutdown([[maybe_unused]] Engine& engine) override { };
};

class SelfDestructModuleLast : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override
    {
        return ModuleTickOrder::eLast;
    };

    void Tick(Engine& engine) override
    {
        engine.RequestShutdown(-2);
    };

    void Shutdown([[maybe_unused]] Engine& engine) override { };
};

class SetAtFreeModule : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override
    {
        return ModuleTickOrder::eFirst;
    }

    void Tick([[maybe_unused]] Engine& engine) override { };

    void Shutdown([[maybe_unused]] Engine& engine) override
    {
        *target = 1;
    };

public:
    bb::u32* target = nullptr;
};

class SetAtFreeModule2 : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override
    {
        return ModuleTickOrder::eFirst;
    }

    void Tick([[maybe_unused]] Engine& engine) override { };

    void Shutdown([[maybe_unused]] Engine& engine) override
    {
        *target = 2;
    };

public:
    bb::u32* target = nullptr;
};

};

TEST(EngineModuleTests, ModuleGetter)
{
    MainEngine e {};
    e.AddModule<TestModules::TestModule>();

    EXPECT_NE(e.TryGetModule<TestModules::TestModule>(), nullptr)
        << "Test Module was not added successfully";
}

TEST(EngineModuleTests, ModuleDependency)
{
    MainEngine e {};
    e.AddModule<TestModules::DependentModule>();

    EXPECT_NE(e.TryGetModule<TestModules::TestModule>(), nullptr)
        << "Test Module was not added, even if it was a dependency of DependantModule";
}

TEST(EngineModuleTests, EngineExit)
{
    MainEngine e {};
    e.AddModule<TestModules::SelfDestructModuleFirst>();

    int* exit = e.Tick();
    ASSERT_TRUE(exit != nullptr);
    EXPECT_EQ(*exit, -1) << "SelfDestructFirst was not able to set the exit code to -1";
}

TEST(EngineModuleTests, EngineReset)
{
    {
        MainEngine e {};
        e.AddModule<TestModules::SelfDestructModuleFirst>();
        int* exit = e.Tick();

        ASSERT_TRUE(exit != nullptr);
        EXPECT_EQ(*exit, -1);
    }

    {
        MainEngine e {};
        e.AddModule<TestModules::SelfDestructModuleFirst>();
        e.AddModule<TestModules::SelfDestructModuleLast>();
        int* exit = e.Tick();

        ASSERT_TRUE(exit != nullptr);
        EXPECT_EQ(*exit, -1);
    }
}

TEST(EngineModuleTests, ModuleDeallocation)
{
    bb::u32 target {};
    {
        MainEngine e {};
        e.GetModule<TestModules::SetAtFreeModule>().target = &target;
        e.GetModule<TestModules::SetAtFreeModule2>().target = &target;
    }
    EXPECT_EQ(target, 1) << "SetAtFreeModule was not deleted last";
    {
        MainEngine e {};
        e.GetModule<TestModules::SetAtFreeModule2>().target = &target;
        e.GetModule<TestModules::SetAtFreeModule>().target = &target;
    }
    EXPECT_EQ(target, 2) << "SetAtFreeModule2 was not deleted last";
}

TEST(EngineModuleTests, ModuleTick)
{
    {
        MainEngine e {};

        e.AddModule<TestModules::CheckUpdateModule>();
        e.AddModule<TestModules::SelfDestructModuleFirst>();

        e.Tick();

        EXPECT_EQ(e.GetModule<TestModules::CheckUpdateModule>()._has_updated, false)
            << "CheckUpdateModule should not have ticked, SelfDestructFirst terminates before";
    }
    {
        MainEngine e {};

        e.AddModule<TestModules::CheckUpdateModule>();
        e.AddModule<TestModules::SelfDestructModuleLast>();

        e.Tick();

        EXPECT_EQ(e.GetModule<TestModules::CheckUpdateModule>()._has_updated, true)
            << "CheckUpdateModule should have ticked, SelfDestructLast terminates afterwards";
    }
}