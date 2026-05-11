#pragma once

#include "common.hpp"
#include "resource_manager.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

class GraphicsContext;
struct GPUImage;

struct UIFont
{
    struct Character
    {
        glm::ivec2 size;
        glm::ivec2 bearing;
        bb::u16 advance;

        glm::vec2 uvMin;
        glm::vec2 uvMax;
    };

    struct FontMetrics
    {
        bb::u16 resolutionY {};
        bb::u16 ascent {};
        bb::u16 descent {};
        bb::u16 lineGap {};
        bb::u16 charSpacing {};
    };

    std::unordered_map<bb::u8, Character> characters;
    ResourceHandle<GPUImage> fontAtlas;
    FontMetrics metrics;
};

[[nodiscard]] std::shared_ptr<UIFont> LoadFromFile(const std::string& path, bb::u16 characterHeight, GraphicsContext& context);
