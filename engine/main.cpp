#include "application_module.hpp"
#include "audio_module.hpp"
#include "ecs_module.hpp"
#include "game_module.hpp"
#include "inspector_module.hpp"
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
#include <log_setup.hpp>

int Main()
{
#ifdef DISTRIBUTION
    bb::setupFileLogger();
#else
    bb::setupDefaultLogger();
#endif

#ifdef DISTRIBUTION
    fileIO::Init(false); // RAII wrapper for mounting the file system.
#else
    fileIO::Init(true); // RAII wrapper for mounting the file system.
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
                .AddModule<InspectorModule>()
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
