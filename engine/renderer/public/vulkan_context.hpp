#pragma once

#include "common.hpp"
#include "vma_include.hpp"
#include "vulkan_fwd.hpp"

#include <bit>
#include <memory>
#include <optional>

struct SDL_Window;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
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
    explicit VulkanContext(SDL_Window* window);

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