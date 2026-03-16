#include "physics/contact_listener.hpp"
#include "Jolt/Physics/Body/Body.h"
#include "components/rigidbody_component.hpp"

JPH::ValidateResult PhysicsContactListener::OnContactValidate(
    [[maybe_unused]] const JPH::Body& inBody1,
    [[maybe_unused]] const JPH::Body& inBody2,
    [[maybe_unused]] JPH::RVec3Arg inBaseOffset,
    [[maybe_unused]] const JPH::CollideShapeResult& inCollisionResult)
{
    // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void PhysicsContactListener::OnContactAdded(
    const JPH::Body& inBody1,
    const JPH::Body& inBody2,
    [[maybe_unused]] const JPH::ContactManifold& inManifold,
    [[maybe_unused]] JPH::ContactSettings& ioSettings)
{
    std::scoped_lock<std::mutex> lock { callbackMutex };

    auto e1 = static_cast<entt::entity>(inBody1.GetUserData());
    auto e2 = static_cast<entt::entity>(inBody2.GetUserData());

    auto rb1 = registry.get<RigidbodyComponent>(e1);
    auto rb2 = registry.get<RigidbodyComponent>(e2);

    rb1.onCollisionEnter(WrenEntity { e1, &registry }, WrenEntity { e2, &registry });
    rb2.onCollisionEnter(WrenEntity { e2, &registry }, WrenEntity { e1, &registry });
}

void PhysicsContactListener::OnContactPersisted(
    const JPH::Body& inBody1,
    const JPH::Body& inBody2,
    [[maybe_unused]] const JPH::ContactManifold& inManifold,
    [[maybe_unused]] JPH::ContactSettings& ioSettings)
{
    std::scoped_lock<std::mutex> lock { callbackMutex };

    auto e1 = static_cast<entt::entity>(inBody1.GetUserData());
    auto e2 = static_cast<entt::entity>(inBody2.GetUserData());

    auto rb1 = registry.get<RigidbodyComponent>(e1);
    auto rb2 = registry.get<RigidbodyComponent>(e2);

    rb1.onCollisionStay(WrenEntity { e1, &registry }, WrenEntity { e2, &registry });
    rb2.onCollisionStay(WrenEntity { e2, &registry }, WrenEntity { e1, &registry });
}

void PhysicsContactListener::OnContactRemoved([[maybe_unused]] const JPH::SubShapeIDPair& inSubShapePair)
{
    // Unimplemented
}