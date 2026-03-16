#pragma once
#include "common.hpp"
#include "physics/collision.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <entt/entity/registry.hpp>

#include <mutex>

class PhysicsContactListener : public JPH::ContactListener
{
public:
    PhysicsContactListener(entt::registry& registry)
        : registry(registry)
    {
    }

    // See: ContactListener
    JPH::ValidateResult OnContactValidate(
        [[maybe_unused]] const JPH::Body& inBody1,
        [[maybe_unused]] const JPH::Body& inBody2,
        [[maybe_unused]] JPH::RVec3Arg inBaseOffset,
        [[maybe_unused]] const JPH::CollideShapeResult& inCollisionResult) override;

    void OnContactAdded(
        [[maybe_unused]] const JPH::Body& inBody1,
        [[maybe_unused]] const JPH::Body& inBody2,
        [[maybe_unused]] const JPH::ContactManifold& inManifold,
        [[maybe_unused]] JPH::ContactSettings& ioSettings) override;

    void OnContactPersisted(
        [[maybe_unused]] const JPH::Body& inBody1,
        [[maybe_unused]] const JPH::Body& inBody2,
        [[maybe_unused]] const JPH::ContactManifold& inManifold,
        [[maybe_unused]] JPH::ContactSettings& ioSettings) override;

    void OnContactRemoved([[maybe_unused]] const JPH::SubShapeIDPair& inSubShapePair) override;

private:
    // contact callbacks are triggered asynchronously
    std::mutex callbackMutex {};
    entt::registry& registry;
};