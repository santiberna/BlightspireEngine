#pragma once
#include "module_interface.hpp"
#include "thread_pool.hpp"

class ThreadModule : public ModuleInterface
{
    ModuleTickOrder Init([[maybe_unused]] Engine& engine) override
    {
        _threadPool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency());
        _threadPool->Start();

        return ModuleTickOrder::eTick; // Module doesn't tick
    }

    void Tick([[maybe_unused]] Engine& engine) override { };
    void Shutdown([[maybe_unused]] Engine& engine) override { };

    std::unique_ptr<ThreadPool> _threadPool;

public:
    NON_COPYABLE(ThreadModule);
    NON_MOVABLE(ThreadModule);

    ThreadModule() = default;
    ~ThreadModule() override = default;

    ThreadPool& GetPool() { return *_threadPool; }
};