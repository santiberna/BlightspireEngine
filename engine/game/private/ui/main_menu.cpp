#include "fonts.hpp"
#include "ui/ui_menus.hpp"
#include "ui_button.hpp"
#include "ui_image.hpp"
#include "ui_text.hpp"

#include <glm/glm.hpp>
#include <resource_management/sampler_resource_manager.hpp>

std::shared_ptr<MainMenu> MainMenu::Create(const bb::UIResources& resources, const glm::uvec2& screenResolution, std::shared_ptr<UIFont> font)
{
    auto main = std::make_shared<MainMenu>(screenResolution);

    main->anchorPoint = UIElement::AnchorPoint::eTopLeft;
    main->SetAbsoluteTransform(main->GetAbsoluteLocation(), screenResolution);

    glm::vec2 screenResFloat = screenResolution;

    // Title
    {
        glm::vec2 pos = glm::vec2(screenResFloat.y * 0.1f);
        glm::vec2 size = glm::vec2(2.0f, 1.0f) * screenResFloat.y * 0.5f;

        auto logoElement = main->AddChild<UIImage>(resources.menu_title, pos, size);
        logoElement->anchorPoint = UIElement::AnchorPoint::eTopLeft;
    }

    // Buttons
    auto buttonPanel = main->AddChild<Canvas>(glm::vec2 { 0.0f, 0.0f });

    {
        buttonPanel->anchorPoint = UIElement::AnchorPoint::eTopLeft;
        buttonPanel->SetLocation(glm::vec2(screenResFloat.y * 0.35f, screenResFloat.y * 0.6f));
    }

    {
        glm::vec2 buttonPos = { 0.0f, 0.0f };
        constexpr glm::vec2 increment = { 0.0f, 140.0f };
        constexpr glm::vec2 buttonBaseSize = glm::vec2(87, 22) * 6.0f;
        constexpr float textSize = 60;

        auto openLinkButton = main->AddChild<UIButton>(resources.button_style, glm::vec2(0), buttonBaseSize);
        openLinkButton->anchorPoint = UIElement::AnchorPoint::eBottomRight;
        openLinkButton->AddChild<UITextElement>(font, "Check out our Discord!", 50);
        main->openLinkButton = openLinkButton;

        auto playButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        playButton->anchorPoint = UIElement::AnchorPoint::eTopLeft;
        auto text = playButton->AddChild<UITextElement>(font, "Play", textSize);

        buttonPos += increment;

        auto controlsButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        controlsButton->anchorPoint = UIElement::AnchorPoint::eTopLeft;
        controlsButton->AddChild<UITextElement>(font, "Controls", textSize);

        buttonPos += increment;

        auto settingsButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        settingsButton->anchorPoint = UIElement::AnchorPoint::eTopLeft;
        settingsButton->AddChild<UITextElement>(font, "Settings", textSize);

        buttonPos += increment;

        auto creditsButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        creditsButton->anchorPoint = UIElement::AnchorPoint::eTopLeft;
        creditsButton->AddChild<UITextElement>(font, "Credits", textSize);

        buttonPos += increment;

        auto quitButton = buttonPanel->AddChild<UIButton>(resources.button_style, buttonPos, buttonBaseSize);
        quitButton->anchorPoint = UIElement::AnchorPoint::eTopLeft;
        quitButton->AddChild<UITextElement>(font, "Quit", textSize);

        main->playButton = playButton;
        main->quitButton = quitButton;
        main->settingsButton = settingsButton;
        main->controlsButton = controlsButton;
        main->creditsButton = creditsButton;

        playButton->navigationTargets.down = main->controlsButton;
        playButton->navigationTargets.up = main->openLinkButton;

        controlsButton->navigationTargets.down = main->settingsButton;
        controlsButton->navigationTargets.up = main->playButton;

        settingsButton->navigationTargets.down = main->creditsButton;
        settingsButton->navigationTargets.up = main->controlsButton;

        creditsButton->navigationTargets.down = main->quitButton;
        creditsButton->navigationTargets.up = main->settingsButton;

        quitButton->navigationTargets.down = main->openLinkButton;
        quitButton->navigationTargets.up = main->creditsButton;

        main->openLinkButton.lock()->navigationTargets.down = main->playButton;
        main->openLinkButton.lock()->navigationTargets.up = main->quitButton;

        main->UpdateAllChildrenAbsoluteTransform();
    }
    return main;
}
