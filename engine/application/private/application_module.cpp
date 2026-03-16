#include "application_module.hpp"
#include "engine.hpp"
#include "input/sdl/sdl_action_manager.hpp"
#include "input/sdl/sdl_input_device_manager.hpp"
#include "input/steam/steam_action_manager.hpp"
#include "input/steam/steam_input_device_manager.hpp"
#include "steam_module.hpp"

#include <spdlog/spdlog.h>

#include <SDL3/SDL.h>
#include <file_io.hpp>
#include <imgui_sdl_include.hpp>
#include <stb_image.h>

ModuleTickOrder ApplicationModule::Init(Engine& engine)
{
    ModuleTickOrder priority = ModuleTickOrder::eLast;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        spdlog::error("Failed initializing SDL: {0}", SDL_GetError());
        engine.RequestShutdown(-1);
        return priority;
    }

    int32_t displayCount {};
    SDL_DisplayID* displayIds = SDL_GetDisplays(&displayCount);
    const SDL_DisplayMode* dm = SDL_GetCurrentDisplayMode(*displayIds);
    glm::ivec2 screenSize { dm->w, dm->h };

    if (dm == nullptr)
    {
        spdlog::error("Failed retrieving DisplayMode: {0}", SDL_GetError());
        engine.RequestShutdown(-1);
        return priority;
    }

    SDL_WindowFlags flags = SDL_WINDOW_VULKAN;
    if (_isFullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    _window = SDL_CreateWindow(_windowName.data(), screenSize.x, screenSize.y, flags);

    if (_window == nullptr)
    {
        spdlog::error("Failed creating SDL window: {}", SDL_GetError());
        engine.RequestShutdown(-1);
        SDL_Quit();
        return priority;
    }

    auto stream = fileIO::OpenReadStream("assets/textures/icon.png");
    int32_t width, height, nrChannels;
    stbi_uc* pixels = fileIO::LoadImageFromIfstream(stream.value(), &width, &height, &nrChannels, 4);
    if (pixels)
    {
        SDL_Surface* icon = SDL_CreateSurfaceFrom(
            width,
            height,
            SDL_PIXELFORMAT_RGBA32,
            pixels, width * 4);
        SDL_SetWindowIcon(_window, icon);

        SDL_DestroySurface(icon);
    }
    else
    {
        spdlog::warn("Unable to load window icon!");
    }

    const SteamModule& steam = engine.GetModule<SteamModule>();
    if (steam.InputAvailable())
    {
        spdlog::info("Steam Input available, creating SteamActionManager. Controller input settings will be used from Steam");
        _inputDeviceManager = std::make_unique<SteamInputDeviceManager>();
        const SteamInputDeviceManager& inputManager = dynamic_cast<SteamInputDeviceManager&>(*_inputDeviceManager);
        _actionManager = std::make_unique<SteamActionManager>(inputManager);
    }
    else
    {
        spdlog::info("Steam Input not available, creating default ActionManager. Controller input settings will be used from program");
        _inputDeviceManager = std::make_unique<SDLInputDeviceManager>();
        const SDLInputDeviceManager& inputManager = dynamic_cast<SDLInputDeviceManager&>(*_inputDeviceManager);
        _actionManager = std::make_unique<SDLActionManager>(inputManager);
    }

    SetMouseHidden(_mouseHidden);

    return priority;
}

void ApplicationModule::Shutdown([[maybe_unused]] Engine& engine)
{
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void ApplicationModule::Tick(Engine& engine)
{
    _inputDeviceManager->Update();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        _inputDeviceManager->UpdateEvent(event);

        if (_mouseHidden == false)
        {
            _inputDeviceManager->SetMousePositionToAbsoluteMousePosition();
        }

        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EventType::SDL_EVENT_QUIT)
        {
            engine.RequestShutdown(0);
            break;
        }
    }

    _actionManager->Update();
}

ApplicationModule::ApplicationModule() = default;

void ApplicationModule::SetMouseHidden(bool val)
{
    _mouseHidden = val;

    // SDL_SetWindowMouseGrab(_window, _mouseHidden);
    SDL_SetWindowRelativeMouseMode(_window, _mouseHidden);

    if (_mouseHidden)
        SDL_HideCursor();
    else
        SDL_ShowCursor();
}
void ApplicationModule::OpenExternalBrowser(const std::string& url)
{
    if (SDL_OpenURL(url.c_str()) == false)
    {
        spdlog::error("Failed opening external browser with url: ", url);
    }
}

glm::uvec2 ApplicationModule::DisplaySize() const
{
    int32_t w {}, h {};
    SDL_GetWindowSize(_window, &w, &h);
    return glm::uvec2 { w, h };
}
bool ApplicationModule::isMinimized() const
{
    SDL_WindowFlags flags = SDL_GetWindowFlags(_window);
    return flags & SDL_WINDOW_MINIMIZED;
}