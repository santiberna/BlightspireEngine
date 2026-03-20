#pragma once

#include <glm/vec2.hpp>
#include <optional>
#include <vma_include.hpp>

#include "common.hpp"
#include "vulkan_fwd.hpp"
#include "vulkan_include.hpp"
#include <memory>

struct SDL_Window;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }

    static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
};

struct VulkanInitInfo
{
    glm::uvec2 window_size {};
    SDL_Window* window_handle = nullptr;
};

enum class BindlessBinding : std::uint8_t
{
    eImage = 0,
    eStorageImage,
    eStorageBuffer,
    eNone,
};

class VulkanContext
{
public:
    explicit VulkanContext(const VulkanInitInfo& initInfo);

    ~VulkanContext();
    NON_COPYABLE(VulkanContext);
    NON_MOVABLE(VulkanContext);

    [[nodiscard]] VkInstance Instance() const;
    [[nodiscard]] VkPhysicalDevice PhysicalDevice() const;
    [[nodiscard]] VkDevice Device() const;
    [[nodiscard]] VkQueue GraphicsQueue() const;
    [[nodiscard]] VkQueue PresentQueue() const;
    [[nodiscard]] VkSurfaceKHR Surface() const;
    [[nodiscard]] VkDescriptorPool DescriptorPool() const;
    [[nodiscard]] VkCommandPool CommandPool() const;
    [[nodiscard]] VmaAllocator MemoryAllocator() const;
    [[nodiscard]] const QueueFamilyIndices& QueueFamilies() const;

    template <typename T>
    void DebugSetObjectName(T vulkanObject, const char* name) const;
    void DebugSetObjectName(void* vulkanObject, uint32_t objectType, const char* name) const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl {};
};

template <typename T>
void VulkanContext::DebugSetObjectName(T vulkanObject, const char* name) const
{
    DebugSetObjectName(std::bit_cast<void*>(vulkanObject), static_cast<uint32_t>(T::objectType), name);
}