#include "ui/ui_menus.hpp"

#include "fonts.hpp"
#include "ui_image.hpp"
#include "ui_progress_bar.hpp"
#include "ui_text.hpp"

std::shared_ptr<HUD> HUD::Create(const bb::UIResources& resources, const glm::uvec2& screenResolution, std::shared_ptr<UIFont> font)
{
    std::shared_ptr<HUD> hud = std::make_shared<HUD>(screenResolution);

    hud->SetAbsoluteTransform(hud->GetAbsoluteLocation(), screenResolution);
    hud->AddChild<UIImage>(resources.hud_crosshair, glm::vec2(0, 7), glm::vec2(25, 42) * 2.0f);

    // stats bg
    auto statsBG = hud->AddChild<UIImage>(resources.hud_background, glm::vec2(0, 0), glm::vec2(113, 39) * 8.0f);
    statsBG->anchorPoint = UIElement::AnchorPoint::eBottomLeft;

    // gun
    auto gunBG = hud->AddChild<UIImage>(resources.hud_gun_bg, glm::vec2(0, 0), glm::vec2(69, 35) * 8.0f);
    gunBG->anchorPoint = UIElement::AnchorPoint::eBottomRight;

    auto gun = hud->AddChild<UIImage>(resources.hud_gun, glm::vec2(16, 12) * 8.0f, glm::vec2(19, 8) * 8.0f);
    gun->anchorPoint = UIElement::AnchorPoint::eBottomRight;

    hud->ammoCounter = hud->AddChild<UITextElement>(font, "17", glm::vec2(43, 10) * 8.0f, 12 * 8.0);
    hud->ammoCounter.lock()->anchorPoint = UIElement::AnchorPoint::eBottomRight;

    // hitmarker
    hud->hitmarker = hud->AddChild<UIImage>(resources.hud_hitmarker, glm::vec2(0, 7), glm::vec2(25, 42) * 2.0f);
    hud->hitmarker.lock()->visibility = UIElement::VisibilityState::eNotUpdatedAndInvisible;

    hud->hitmarkerCrit = hud->AddChild<UIImage>(resources.hud_hitmarker_crit, glm::vec2(0, 7), glm::vec2(25, 42) * 2.0f);
    hud->hitmarkerCrit.lock()->visibility = UIElement::VisibilityState::eNotUpdatedAndInvisible;

    // dashes
    for (size_t i = 0; i < hud->dashCharges.size(); i++)
    {
        hud->dashCharges[i] = hud->AddChild<UIImage>(resources.hud_dash_charge, (glm::vec2(21 + 9.f * i, 19) * 8.0f), glm::vec2(6.f) * 8.f);
        hud->dashCharges[i].lock()->anchorPoint = UIElement::AnchorPoint::eBottomLeft;
    }

    // healthbar
    hud->healthBar = hud->AddChild<UIProgressBar>(resources.health_bar_style, glm::vec2(10, 11) * 8.0f, glm::vec2(94, 5) * 8.0f);
    hud->healthBar.lock()->anchorPoint = UIElement::AnchorPoint::eBottomLeft;

    // souls indicator
    hud->soulIndicator = hud->AddChild<UIImage>(resources.hud_soul_indicator, glm::vec2(11, 18) * 8.0f, glm::vec2(5, 8) * 8.0f);
    hud->soulIndicator.lock()->anchorPoint = UIElement::AnchorPoint::eBottomLeft;

    hud->AddChild<UIImage>(resources.hud_wave_bg, glm::vec2(0, 0) * 8.0f, glm::vec2(59, 54) * 8.0f)->anchorPoint = UIElement::AnchorPoint::eTopRight;
    hud->AddChild<UIImage>(resources.hud_coin_icon, glm::vec2(49, 20) * 8.0f + glm::vec2(300.f, 0), glm::vec2(4 * 8))->anchorPoint = UIElement::AnchorPoint::eBottomLeft;

    for (auto& i : hud->directionalIndicators)
    {
        i = hud->AddChild<UIImage>(resources.hud_directional_damage_icon, glm::vec2(0, 0), glm::vec2(21, 6) * 8.0f);
        i.lock()->display_color = glm::vec4(0);
    }

    // wave counter

    hud->waveCounterText = hud->AddChild<UITextElement>(font, "0", glm::vec2(140, 26), 20 * 8.0);
    hud->waveCounterText.lock()->anchorPoint = UIElement::AnchorPoint::eTopRight;
    hud->waveCounterText.lock()->display_color = glm::vec4(0.459 * 2.0, 0.31 * 2.0, 0.28 * 2.0, 1);

    hud->scoreText = hud->AddChild<UITextElement>(font, "220", glm::vec2(445 + 300, 142), 10 * 8.0);
    hud->scoreText.lock()->anchorPoint = UIElement::AnchorPoint::eBottomLeft;

    // Power up

    hud->powerupText = hud->AddChild<UITextElement>(font, "220", glm::vec2(0, -320), 10 * 12.0);
    hud->powerupText.lock()->anchorPoint = UIElement::AnchorPoint::eMiddle;

    hud->powerUpTimer = hud->AddChild<UITextElement>(font, "Timer", glm::vec2(0, -520), 10 * 16.0);
    hud->powerUpTimer.lock()->anchorPoint = UIElement::AnchorPoint::eMiddle;

    // Action binding visual

    hud->actionBindingText = hud->AddChild<UITextElement>(font, "Binding", glm::vec2(0, 100), 70);
    hud->actionBindingText.lock()->anchorPoint = UIElement::AnchorPoint::eMiddle;
    hud->actionBindingText.lock()->visibility = VisibilityState::eNotUpdatedAndInvisible;

    hud->actionBindingGlyph = hud->AddChild<UIImage>(resources.hud_gun, glm::vec2(-130, 100), glm::vec2(50, 50));
    hud->actionBindingGlyph.lock()->anchorPoint = UIElement::AnchorPoint::eMiddle;
    hud->actionBindingGlyph.lock()->visibility = VisibilityState::eNotUpdatedAndInvisible;

    hud->UpdateAllChildrenAbsoluteTransform();
    return hud;
}
