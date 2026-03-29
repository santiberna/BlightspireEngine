#include "engine.hpp"
#include "game_module.hpp"
#include "ui/ui_menus.hpp"
#include "ui_image.hpp"
#include "ui_module.hpp"
#include "ui_text.hpp"

std::shared_ptr<CreditsMenu> CreditsMenu::Create(
    Engine& engine,
    const bb::UIResources& resources,
    const glm::uvec2& screenResolution,
    std::shared_ptr<UIFont> font)
{
    auto credits = std::make_shared<CreditsMenu>(screenResolution);

    credits->anchorPoint = UIElement::AnchorPoint::eMiddle;
    credits->SetAbsoluteTransform(credits->GetAbsoluteLocation(), screenResolution);

    auto image = credits->AddChild<UIImage>(resources.partial_black_bg, glm::vec2(), glm::vec2());
    image->anchorPoint = UIElement::AnchorPoint::eFill;

    // Title
    {
        credits->AddChild<UITextElement>(font, "The Bubonic Brotherhood", glm::vec2 { 0.0f, -575.0f }, 100);
    }

    constexpr float textSize = 55;

    glm::vec2 elemPos = { 0.0f, -550.0f };
    constexpr glm::vec2 rowSize = { 900.0f, 0.0f };
    constexpr glm::vec2 increment = { 0.0f, 80.0f };

    constexpr glm::vec2 buttonBaseSize = glm::vec2(87, 22) * 5.0f;

    std::pair<const char*, const char*> bubonic_brotherhood[] = {
        { "Ferri de Lange", "Lead / Graphics" },
        { "Pawel Trajdos", "Producer / Designer" },
        { "Luuk Asmus", "Gameplay / Engine" },
        { "Leo Hellmig", "Gameplay / Engine" },
        { "Lon van Ettinger", "Graphics / QA" },
        { "Marcin Zalewski", "Engine / Graphics" },
        { "Rares Dumitru", "Gameplay / Graphics" },
        { "Lars Janssen", "Engine / Trailer" },
        { "Sikandar Maksoedan", "Setdressing / Tech art" },
        { "Vengar van de Wal", "Graphics" },
        { "Santiago Bernardino", "Engine / QA" },
    };

    {
        for (auto& elem : bubonic_brotherhood)
        {
            elemPos += increment;

            auto node = credits->AddChild<Canvas>(rowSize);
            node->SetLocation(elemPos);

            auto name1 = node->AddChild<UITextElement>(font, elem.first, glm::vec2(), textSize);
            name1->anchorPoint = UIElement::AnchorPoint::eTopLeft;

            auto name2 = node->AddChild<UITextElement>(font, elem.second, glm::vec2(), textSize);
            name2->anchorPoint = UIElement::AnchorPoint::eTopRight;
        }
    }

    // BACK BUTTON
    {
        constexpr glm::vec2 backPos = { 0.0f, 525.0f };
        auto back = credits->AddChild<UIButton>(resources.button_style, backPos, buttonBaseSize);
        back->anchorPoint = UIElement::AnchorPoint::eMiddle;
        back->AddChild<UITextElement>(font, "Back", textSize);

        auto callback = [&engine]()
        {
            auto& gameModule = engine.GetModule<GameModule>();
            engine.GetModule<UIModule>().uiInputContext.focusedUIElement = gameModule.PopPreviousFocusedElement();
            gameModule.GetSettings().SaveToFile(GAME_SETTINGS_FILE);
            gameModule.PopUIMenu();
        };

        back->OnPress(Callback { callback });
        credits->backButton = back;
    }

    credits->UpdateAllChildrenAbsoluteTransform();
    return credits;
}
