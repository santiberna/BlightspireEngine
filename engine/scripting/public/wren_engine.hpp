#pragma once
#include "engine.hpp"

#include <optional>

// Wrapper class for Accessing the engine
struct WrenEngine
{
    Engine* instance {};

    template <typename T>
    std::optional<T*> GetModule()
    {
        if (instance == nullptr)
            return std::nullopt;

        if (auto ptr = instance->TryGetModule<T>())
            return ptr;

        return std::nullopt;
    }
};