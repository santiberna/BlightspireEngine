#pragma once
#include "common.hpp"

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

class PerformanceTracker
{
public:
    PerformanceTracker();
    void Update();
    void Render();

private:
    static const bb::u32 MAX_SAMPLES { 512 };

    std::vector<float> _fpsValues {};

    std::vector<float> _frameDurations {};
    std::vector<std::string> _labels {};

    std::vector<float> _timePoints {};
    std::chrono::steady_clock::time_point _lastFrameTime {};
    float _totalTime {};
    bb::u32 _frameCounter {};

    float _highestFps {};
    bb::u32 _highestFpsRecordIndex {};
    float _highestFrameDuration {};
    bb::u32 _highestFrameDurationRecordIndex {};
};