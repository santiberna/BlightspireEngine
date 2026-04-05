#include "application_module.hpp"
#include "audio_module.hpp"
#include "ecs_module.hpp"
#include "game_module.hpp"
#include "log_setup.hpp"
#include "main_engine.hpp"
#include "particle_module.hpp"
#include "pathfinding_module.hpp"
#include "physfs.hpp"
#include "physics_module.hpp"
#include "renderer_module.hpp"
#include "scripting_module.hpp"
#include "steam_module.hpp"
#include "thread_module.hpp"
#include "time_module.hpp"
#include "ui_module.hpp"

#include <spdlog/spdlog.h>

#if BB_DEVELOPMENT
#include "inspector_module.hpp"
#endif

int Main()
{
#if BB_DEVELOPMENT
    fileIO::Init(true); // RAII wrapper for mounting the file system.
    bb::setupDefaultLogger();
#else
    fileIO::Init(false); // RAII wrapper for mounting the file system.
    bb::setupFileLogger();
#endif
    int result;
    {
        MainEngine instance;
        Stopwatch startupTimer {};

        {
            ZoneScopedN("Engine Module Initialization");

            instance
                .AddModule<ThreadModule>()
                .AddModule<ECSModule>()
                .AddModule<TimeModule>()
                .AddModule<SteamModule>()
                .AddModule<ApplicationModule>()
                .AddModule<PhysicsModule>()
                .AddModule<RendererModule>()
                .AddModule<PathfindingModule>()
                .AddModule<AudioModule>()
                .AddModule<UIModule>()
                .AddModule<ParticleModule>()
                .AddModule<GameModule>()
#if BB_DEVELOPMENT
                .AddModule<InspectorModule>()
#endif
                .AddModule<ScriptingModule>();
        }

        {
            ZoneScopedN("Game Script Setup");
            auto& scripting = instance.GetModule<ScriptingModule>();

            scripting.ResetVM();
            scripting.GenerateEngineBindingsFile();
            scripting.SetMainScript(instance, "game/main_menu.wren");

            instance.GetModule<TimeModule>().ResetTimer();
        }

        spdlog::info("{}ms taken for complete startup!", startupTimer.GetElapsed().count());

        int* exit = nullptr;
        while (exit == nullptr)
        {
            exit = instance.Tick();
        }
        result = *exit;
    }

    fileIO::Deinit();
    return result;
}

#if (BB_PLATFORM == BB_WINDOWS) && (BB_DEVELOPMENT == 0)

int APIENTRY WinMain([[maybe_unused]] HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] LPSTR lpCmdLine, [[maybe_unused]] int nShowCmd)
{
    return Main();
}

#else

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    return Main();
}

#endif
