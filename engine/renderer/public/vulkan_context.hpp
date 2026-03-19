#pragma once

#include <glm/vec2.hpp>
#include <memory>
#include <optional>
#include <vma_include.hpp>

#include "common.hpp"
#include "vulkan_include.hpp"

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

    vk::Instance Instance() const { return _instance; }
    vk::PhysicalDevice PhysicalDevice() const { return _physicalDevice; }
    vk::Device Device() const { return _device; }
    vk::Queue GraphicsQueue() const { return _graphicsQueue; }
    vk::Queue PresentQueue() const { return _presentQueue; }
    vk::SurfaceKHR Surface() const { return _surface; }
    vk::DescriptorPool DescriptorPool() const { return _descriptorPool; }
    vk::CommandPool CommandPool() const { return _commandPool; }
    VmaAllocator MemoryAllocator() const { return _vmaAllocator; }

    [[nodiscard]] const QueueFamilyIndices& QueueFamilies() const { return _queueFamilyIndices; }

private:
    friend class Renderer;

    vk::Instance _instance;
    vk::PhysicalDevice _physicalDevice;
    vk::Device _device;
    vk::Queue _graphicsQueue;
    vk::Queue _presentQueue;
    vk::SurfaceKHR _surface;
    vk::DescriptorPool _descriptorPool;
    vk::CommandPool _commandPool;
    VmaAllocator _vmaAllocator;
    QueueFamilyIndices _queueFamilyIndices;
    uint32_t _minUniformBufferOffsetAlignment;

#if BB_DEVELOPMENT
    vk::DebugUtilsMessengerEXT _debugMessenger;
#endif

    void PickPhysicalDevice();
    uint32_t RateDeviceSuitability(const vk::PhysicalDevice& device);

    bool ExtensionsSupported(const vk::PhysicalDevice& device);

    void CreateInstance();
    void CreateDevice();
    void CreateCommandPool();
    void CreateDescriptorPool();
};