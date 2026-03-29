#include "fmod_debug.hpp"
#include "fmod_include.hpp"

#include <spdlog/spdlog.h>

void FMOD_CHECKRESULT_fn(FMOD_RESULT result, [[maybe_unused]] const char* file, int line)
{
    if (result != FMOD_OK)
    {
        spdlog::error("FMOD ERROR: audio_module.cpp [Line {0} ] {1} - {2}", line, static_cast<int>(result), FMOD_ErrorString(result));
        throw std::runtime_error(FMOD_ErrorString(result));
    }
}

#if BB_DEVELOPMENT
FMOD_RESULT DebugCallback(FMOD_DEBUG_FLAGS flags, [[maybe_unused]] const char* file, int line, const char* func, const char* message)
{
    // We use std::cout instead of using spdlog because otherwise it crashes 💀 (some threading issue with fmod)
    switch (flags)
    {
    case FMOD_DEBUG_LEVEL_LOG:
        spdlog::info("[FMOD] {} ({}) - {}", message, func, line);
        break;
    case FMOD_DEBUG_LEVEL_WARNING:
        spdlog::warn("[FMOD] {} ({}) - {}", message, func, line);
        break;
    case FMOD_DEBUG_LEVEL_ERROR:
        spdlog::error("[FMOD] {} ({}) - {}", message, func, line);
        break;
    default:
        spdlog::debug("[FMOD] {} ({}) - {}", message, func, line);
        break;
    }

    return FMOD_OK;
}
#endif

void StartFMODDebugLogger()
{
#if BB_DEVELOPMENT
    // Use FMOD_DEBUG_LEVEL_MEMORY if you want to debug memory issues related to fmod
    FMOD_RESULT result = FMOD_Debug_Initialize(FMOD_DEBUG_LEVEL_WARNING, FMOD_DEBUG_MODE_CALLBACK, &DebugCallback, nullptr);

    if (result != FMOD_OK)
    {
        spdlog::error("FMOD Error: {0}", FMOD_ErrorString(result));
    }
#endif
}