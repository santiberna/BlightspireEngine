#include "ui/resources.hpp"

#include "graphics_resources.hpp"
#include "resource_management/image_resource_manager.hpp"
#include "resources/sampler.hpp"

bb::UIResources bb::UIResources::Load(GraphicsContext& context)
{
    UIResources out {};
    SingleTimeCommands upload { *context.GetVulkanContext() };

    auto& sampler_manager = context.Resources()->GetSamplerResourceManager();
    auto& texture_manager = context.Resources()->GetImageResourceManager();

    // Sampler for UI images
    {
        SamplerCreation info;
        info.minFilter = bb::SamplerFilter::NEAREST;
        info.magFilter = bb::SamplerFilter::NEAREST;
        out.ui_sampler = sampler_manager.Create(info, "UI Sampler");
    }

    // Black backdrop
    {
        bb::Image2D background {};
        background.data = std::make_shared<std::byte[]>(4);
        background.format = ImageFormat::R8G8B8A8_UNORM;
        background.width = 1;
        background.height = 1;
        background.data[0] = std::byte(0);
        background.data[1] = std::byte(0);
        background.data[2] = std::byte(0);
        background.data[3] = std::byte(150);

        out.partial_black_bg = texture_manager.Create(
            background, out.ui_sampler, TextureFlags::COMMON_FLAGS, "PartialBlackBackdrop", &upload);

        background.data[3] = std::byte(255);
        out.full_black_bg = texture_manager.Create(
            background, out.ui_sampler, TextureFlags::COMMON_FLAGS, "FullBlackBackdrop", &upload);
    }

    auto load_texture = [&](std::string_view path)
    {
        auto image = bb::Image2D::fromFile(path).value();
        return texture_manager.Create(
            image, out.ui_sampler, TextureFlags::COMMON_FLAGS, path, &upload);
    };

    // Button Style
    {
        out.button_style.normalImage = load_texture("assets/textures/ui/button.png");
        out.button_style.hoveredImage = load_texture("assets/textures/ui/button_2.png");
        out.button_style.pressedImage = load_texture("assets/textures/ui/button_selected.png");
    }

    // Toggle Style
    {
        out.toggle_style.empty = load_texture("assets/textures/ui/toggle_empty.png");
        out.toggle_style.filled = load_texture("assets/textures/ui/toggle_full.png");
    }

    // Slider Style
    {
        out.slider_style.empty = load_texture("assets/textures/ui/slider_empty.png");
        out.slider_style.filled = load_texture("assets/textures/ui/slider_full.png");
        out.slider_style.knob = load_texture("assets/textures/ui/slider_knob.png");
    }

    // Health bar
    {
        out.health_bar_style.empty = {};
        out.health_bar_style.filled = load_texture("assets/textures/ui/health_bar.png");
        out.health_bar_style.fillStyle = UIProgressBar::BarStyle::FillStyle::eMask;
        out.health_bar_style.fillDirection = UIProgressBar::BarStyle::FillDirection::eLeftToRight;
    }

    // Controls BG
    {
        out.controls_bg = load_texture("assets/textures/ui/popup_background.png");
    }

    out.hud_crosshair = load_texture("assets/textures/ui/cross_hair.png");
    out.hud_background = load_texture("assets/textures/ui/stats_bg.png");
    out.hud_gun_bg = load_texture("assets/textures/ui/gun_bg.png");
    out.hud_gun = load_texture("assets/textures/ui/gun.png");
    out.hud_hitmarker = load_texture("assets/textures/ui/hitmarker.png");
    out.hud_hitmarker_crit = load_texture("assets/textures/ui/hitmarker_crit.png");
    out.hud_dash_charge = load_texture("assets/textures/ui/dash_charge.png");
    out.hud_soul_indicator = load_texture("assets/textures/ui/souls_indicator.png");
    out.hud_wave_bg = load_texture("assets/textures/ui/wave_bg.png");
    out.hud_coin_icon = load_texture("assets/textures/ui/coin.png");
    out.hud_directional_damage_icon = load_texture("assets/textures/ui/direction.png");
    out.menu_title = load_texture("assets/textures/blightspire_logo.png");

    context.UpdateBindlessSet();
    return out;
}