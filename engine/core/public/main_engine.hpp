#pragma once
#include "engine.hpp"

// Engine subclass meant to run and exit the application
class MainEngine : public Engine
{
public:
    virtual ~MainEngine() = default;
    // Executes tick loop once. Returns exit code if exit was requested, nullptr otherwise
    int* Tick();
};