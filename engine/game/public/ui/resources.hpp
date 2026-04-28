#pragma once
#include "graphics_context.hpp"
#include "ui_button.hpp"
#include "ui_progress_bar.hpp"
#include "ui_slider.hpp"
#include "ui_toggle.hpp"

namespace bb
{

struct UIResources
{
    static UIResources Load(GraphicsContext& context);

    ResourceHandle<bb::Sampler> ui_sampler;
    ResourceHandle<GPUImage> partial_black_bg;
    ResourceHandle<GPUImage> full_black_bg;
    ResourceHandle<GPUImage> hud_crosshair;
    ResourceHandle<GPUImage> hud_background;
    ResourceHandle<GPUImage> hud_gun_bg;
    ResourceHandle<GPUImage> hud_gun;
    ResourceHandle<GPUImage> hud_hitmarker;
    ResourceHandle<GPUImage> hud_hitmarker_crit;
    ResourceHandle<GPUImage> hud_dash_charge;
    ResourceHandle<GPUImage> hud_soul_indicator;
    ResourceHandle<GPUImage> hud_wave_bg;
    ResourceHandle<GPUImage> hud_coin_icon;
    ResourceHandle<GPUImage> hud_directional_damage_icon;
    ResourceHandle<GPUImage> menu_title;
    ResourceHandle<GPUImage> controls_bg;
    UIButton::ButtonStyle button_style;
    UIProgressBar::BarStyle health_bar_style;
    UIToggle::ToggleStyle toggle_style {};
    UISlider::SliderStyle slider_style {};
};

}