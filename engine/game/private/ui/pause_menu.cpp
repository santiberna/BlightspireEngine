#include "fonts.hpp"
#include "ui/ui_menus.hpp"
#include "ui_button.hpp"
#include "ui_image.hpp"
#include "ui_text.hpp"

#include <glm/glm.hpp>

std::shared_ptr<PauseMenu> PauseMenu::Create(const bb::UIResources& resources, const glm::uvec2& screenResolution, std::shared_ptr<UIFont> font)
{
    auto pause = std::make_shared<PauseMenu>(screenResolution);
    pause->anchorPoint = UIElement::AnchorPoint::eMiddle;
    pause->SetAbsoluteTransform(pause->GetAbsoluteLocation(), screenResolution);

    {
        auto image = pause->AddChild<UIImage>(resources.partial_black_bg, glm::vec2(), glm::vec2());
        image->anchorPoint = UIElement::AnchorPoint::eFill;
    }

    auto buttonPanel = pause->AddChild<Canvas>(glm::vec2 { 0.0f, 0.0f });

    {
        buttonPanel->anchorPoint = UIElement::AnchorPoint::eMiddle;
        // buttonPanel->SetLocation(glm::vec2(screenResFloat.y * 0.1f, screenResFloat.y * 0.4f));
    }

    {
        glm::vec2 buttonPos = { 0.0f, -300.0f };
        constexpr glm::vec2 increment = { 0.0f, 140.0f };
        constexpr glm::vec2 buttonBaseSize = glm::vec2(87, 22) * 6.0f;
        constexpr float textSize = 60;

        buttonPanel->AddChild<UITextElement>(font, "Paused", buttonPos, 200);
        buttonPos += increment * 2.0f;

        auto continueButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        continueButton->anchorPoint = UIElement::AnchorPoint::eMiddle;
        continueButton->AddChild<UITextElement>(font, "Continue", textSize);

        buttonPos += increment;

        auto controlsButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        controlsButton->anchorPoint = UIElement::AnchorPoint::eMiddle;
        controlsButton->AddChild<UITextElement>(font, "Controls", textSize);

        buttonPos += increment;

        auto settingsButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        settingsButton->anchorPoint = UIElement::AnchorPoint::eMiddle;
        settingsButton->AddChild<UITextElement>(font, "Settings", textSize);

        buttonPos += increment;

        auto backToMainButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        backToMainButton->anchorPoint = UIElement::AnchorPoint::eMiddle;
        backToMainButton->AddChild<UITextElement>(font, "Main Menu", textSize);

        pause->continueButton = continueButton;
        pause->backToMainButton = backToMainButton;
        pause->settingsButton = settingsButton;
        pause->controlsButton = controlsButton;

        continueButton->navigationTargets.down = pause->controlsButton;
        continueButton->navigationTargets.up = pause->backToMainButton;

        controlsButton->navigationTargets.down = pause->settingsButton;
        controlsButton->navigationTargets.up = pause->continueButton;

        settingsButton->navigationTargets.down = pause->backToMainButton;
        settingsButton->navigationTargets.up = pause->controlsButton;

        backToMainButton->navigationTargets.down = pause->continueButton;
        backToMainButton->navigationTargets.up = pause->settingsButton;
    }

    pause->UpdateAllChildrenAbsoluteTransform();
    return pause;
}
