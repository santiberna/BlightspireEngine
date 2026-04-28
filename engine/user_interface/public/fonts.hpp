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
        uint16_t advance;

        glm::vec2 uvMin;
        glm::vec2 uvMax;
    };

    struct FontMetrics
    {
        uint16_t resolutionY {};
        uint16_t ascent {};
        uint16_t descent {};
        uint16_t lineGap {};
        uint16_t charSpacing {};
    };

    std::unordered_map<uint8_t, Character> characters;
    ResourceHandle<GPUImage> fontAtlas;
    FontMetrics metrics;
};

[[nodiscard]] std::shared_ptr<UIFont> LoadFromFile(const std::string& path, uint16_t characterHeight, GraphicsContext& context);
