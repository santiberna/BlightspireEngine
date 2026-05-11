#pragma once
#include "common.hpp"

#include "module_interface.hpp"
#include "system_interface.hpp"
#include "utility/entity_serializer.hpp"

#include <cassert>
#include <entt/entity/registry.hpp>

struct DeleteTag
{
};

class ECSModule : public ModuleInterface
{
    ModuleTickOrder Init(Engine& engine) override;
    void Shutdown(Engine& engine) override;
    void Tick(Engine& engine) override;

    void UpdateSystems(float dt);
    void RenderSystems() const;
    void RemovedDestroyed();

public:
    void DestroyEntity(entt::entity entity);

    ECSModule() = default;
    ~ECSModule() override = default;

    NON_COPYABLE(ECSModule);
    NON_MOVABLE(ECSModule);

    entt::registry& GetRegistry() { return registry; }
    const entt::registry& GetRegistry() const { return registry; }
    std::vector<std::unique_ptr<SystemInterface>>& GetSystems() { return systems; }

    template <typename T, typename... Args>
    void AddSystem(Args&&... args)
        requires std::is_base_of_v<SystemInterface, T>
    {
        systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    // Returns nullptr if no System T is found
    template <typename T>
    T* GetSystem()
        requires std::is_base_of_v<SystemInterface, T>
    {
        for (auto& s : systems)
        {
            if (auto* found = dynamic_cast<T*>(s.get()))
                return found;
        }
        assert(false && "Could not find system");
        return nullptr;
    }

    template <typename A>
    void save(A& archive, [[maybe_unused]] bb::u32 version) const;

private:
    entt::registry registry {};
    std::vector<std::unique_ptr<SystemInterface>> systems {};
};

template <typename A>
void ECSModule::save(A& archive, [[maybe_unused]] bb::u32 version) const
{
    auto entityView = registry.view<entt::entity>();
    for (auto entity : entityView)
    {
        archive(EntitySerializer(registry, entity));
    }
}

CEREAL_CLASS_VERSION(ECSModule, 0);