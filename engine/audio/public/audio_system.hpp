#pragma once
#include "common.hpp"

#include "system_interface.hpp"

class AudioModule;
class ECSModule;

class AudioSystem final : public SystemInterface
{
public:
    AudioSystem(ECSModule& ecs, AudioModule& audioModule);
    ~AudioSystem() override = default;
    NON_COPYABLE(AudioSystem);
    NON_MOVABLE(AudioSystem);

    void Update(ECSModule& ecs, [[maybe_unused]] float dt) override;
    void Render([[maybe_unused]] const ECSModule& ecs) const override { }
    void Inspect() override;

    std::string_view GetName() override { return "AudioSystem"; }

private:
    [[maybe_unused]] ECSModule& _ecs;
    AudioModule& _audioModule;
};
