#include "ui/ui_menus.hpp"
#include "ui_image.hpp"
#include "ui_text.hpp"

std::shared_ptr<GameOverMenu> GameOverMenu::Create(const bb::UIResources& resources, const glm::uvec2& screenResolution, std::shared_ptr<UIFont> font)
{
    auto over = std::make_shared<GameOverMenu>(screenResolution);
    over->anchorPoint = UIElement::AnchorPoint::eMiddle;
    over->SetAbsoluteTransform(over->GetAbsoluteLocation(), screenResolution);

    auto image = over->AddChild<UIImage>(resources.partial_black_bg, glm::vec2(), glm::vec2());
    image->anchorPoint = UIElement::AnchorPoint::eFill;

    auto buttonPanel = over->AddChild<Canvas>(glm::vec2 { 0.0f, 0.0f });

    {
        buttonPanel->anchorPoint = UIElement::AnchorPoint::eMiddle;
        // buttonPanel->SetLocation(glm::vec2(screenResFloat.y * 0.1f, screenResFloat.y * 0.4f));
    }

    {
        glm::vec2 buttonPos = { 0.0f, -300.0f };
        constexpr glm::vec2 increment = { 0.0f, 140.0f };
        constexpr glm::vec2 buttonBaseSize = glm::vec2(87, 22) * 6.0f;
        constexpr float textSize = 60;

        buttonPanel->AddChild<UITextElement>(font, "Game Over!", buttonPos, 200);
        buttonPos += increment * 2.0f;

        auto continueButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        continueButton->anchorPoint = UIElement::AnchorPoint::eMiddle;
        continueButton->AddChild<UITextElement>(font, "Retry", textSize);

        buttonPos += increment;

        auto backToMainButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        backToMainButton->anchorPoint = UIElement::AnchorPoint::eMiddle;
        backToMainButton->AddChild<UITextElement>(font, "Main Menu", textSize);

        over->continueButton = continueButton;
        over->backToMainButton = backToMainButton;

        continueButton->navigationTargets.down = over->backToMainButton;
        continueButton->navigationTargets.up = over->backToMainButton;

        backToMainButton->navigationTargets.down = over->continueButton;
        backToMainButton->navigationTargets.up = over->continueButton;
    }

    over->UpdateAllChildrenAbsoluteTransform();
    return over;
}