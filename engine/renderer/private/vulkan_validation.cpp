#include "vulkan_validation.hpp"
#include "common.hpp"

void util::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = vk::DebugUtilsMessengerCreateInfoEXT {};
    createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
    createInfo.pfnUserCallback = DebugCallback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL util::DebugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT messageType,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    MAYBE_UNUSED void* pUserData)
{
    const char* typeText = "[UNKNOWN]";

    if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
    {
        typeText = "[GENERAL]";
    }
    else if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
    {
        typeText = "[VALIDATION]";
    }
    else if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
    {
        typeText = "[PERFORMANCE]";
    }

    spdlog::level::level_enum logLevel {};

    switch (messageSeverity)
    {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
        logLevel = spdlog::level::level_enum::trace;
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
        logLevel = spdlog::level::level_enum::info;
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        logLevel = spdlog::level::level_enum::warn;
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        logLevel = spdlog::level::level_enum::err;
        break;
    default:
        break;
    }

    if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        spdlog::log(logLevel, "{0} Validation layer: {1}", typeText, pCallbackData->pMessage);

    if (logLevel == spdlog::level::err)
    {
        // assert(false && "Vulkan Error: check logs");
    }

    return VK_FALSE;
}