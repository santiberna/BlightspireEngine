#include "vulkan_context.hpp"

#include <map>
#include <set>
#include <unordered_set>

#include "pipeline_builder.hpp"
#include "swap_chain.hpp"
#include "vulkan_helper.hpp"
#include <log_setup.hpp>

#include <SDL3/SDL_vulkan.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace
{
constexpr uint32_t VULKAN_API_VERSION = vk::makeApiVersion(0, 1, 4, 0);

#if BB_DEVELOPMENT
constexpr const char* VALIDATION_LAYER = "VK_LAYER_KHRONOS_validation";
#endif

constexpr const char* INSTANCE_EXTENSIONS[] = {
#if BB_DEVELOPMENT
    vk::EXTDebugUtilsExtensionName
#endif
#if BB_PLATFORM == BB_LINUX
        vk::KHRGetPhysicalDeviceProperties2ExtensionName,
#endif
};

constexpr std::array DEVICE_EXTENSIONS = {
    vk::KHRSwapchainExtensionName,
    vk::KHRCreateRenderpass2ExtensionName,
    vk::KHRDepthStencilResolveExtensionName,
    vk::KHRDynamicRenderingExtensionName,
    vk::EXTDescriptorIndexingExtensionName,
    vk::KHRShaderDrawParametersExtensionName,
    vk::KHRDrawIndirectCountExtensionName,
    vk::EXTCalibratedTimestampsExtensionName,
    vk::KHRPushDescriptorExtensionName,
    vk::EXTSamplerFilterMinmaxExtensionName,

#if BB_PLATFORM == BB_LINUX
    vk::KHRMaintenance2ExtensionName,
    vk::KHRMultiviewExtensionName,
#endif
};

#if BB_DEVELOPMENT
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT messageType,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData)
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

    // Do not abort or assert(false) here.
    // Validation errors are not fatal and the application can output extra debug logs in some circumstances after this is called.
    // Prefer breakpoints in the debugger if needed

    return VK_FALSE;
}
#endif

bool ExtensionsSupported(vk::PhysicalDevice deviceToCheckSupport)
{
    std::vector<vk::ExtensionProperties> availableExtensions = deviceToCheckSupport.enumerateDeviceExtensionProperties().value;
    std::set<std::string> requiredExtensions { DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end() };
    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

bool IsComplete(QueueFamilyIndices& indices)
{
    return indices.graphicsFamily && indices.presentFamily;
}

QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
    QueueFamilyIndices indices {};

    uint32_t queueFamilyCount { 0 };
    device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

    std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
    device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

    for (size_t i = 0; i < queueFamilies.size(); ++i)
    {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphicsFamily = i;

        if (!indices.presentFamily.has_value())
        {
            vk::Bool32 supported;
            util::VK_ASSERT(device.getSurfaceSupportKHR(i, surface, &supported),
                "Failed querying surface support on physical device!");
            if (supported)
                indices.presentFamily = i;
        }

        if (indices.presentFamily && indices.graphicsFamily)
            break;
    }

    return indices;
}

uint32_t RateDeviceSuitability(vk::PhysicalDevice deviceToRate, vk::SurfaceKHR surface)
{
    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceDescriptorIndexingFeatures> structureChain;

    auto& indexingFeatures = structureChain.get<vk::PhysicalDeviceDescriptorIndexingFeatures>();
    auto& deviceFeatures = structureChain.get<vk::PhysicalDeviceFeatures2>();

    vk::PhysicalDeviceProperties deviceProperties;
    deviceToRate.getProperties(&deviceProperties);
    deviceToRate.getFeatures2(&deviceFeatures);

    QueueFamilyIndices familyIndices = FindQueueFamilies(deviceToRate, surface);

    uint32_t score { 0 };

    // Failed if geometry shader is not supported.
    if (!deviceFeatures.features.geometryShader)
        return 0;

    // Failed if graphics family queue is not supported.
    if (!IsComplete(familyIndices))
        return 0;

    // Failed if no extensions are supported.
    if (!ExtensionsSupported(deviceToRate))
        return 0;

    // Check for bindless rendering support.
    if (!indexingFeatures.descriptorBindingPartiallyBound || !indexingFeatures.runtimeDescriptorArray || !indexingFeatures.descriptorBindingUniformBufferUpdateAfterBind)
        return 0;

    // Check support for swap chain.
    SwapChain::SupportDetails swapChainSupportDetails = SwapChain::QuerySupport(deviceToRate, surface);
    bool swapChainUnsupported = swapChainSupportDetails.formats.empty() || swapChainSupportDetails.presentModes.empty();
    if (swapChainUnsupported)
        return 0;

    // Favor discrete GPUs above all else.
    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        score += 50000;

    // Slightly favor integrated GPUs.
    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
        score += 30000;

    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

}
struct VulkanContext::Impl
{
    vk::Instance instance;
    vk::PhysicalDevice physical_device;
    vk::Device device;
    vk::Queue graphics_queue;
    vk::Queue present_queue;
    vk::SurfaceKHR surface;
    vk::DescriptorPool descriptor_pool;
    vk::CommandPool command_pool;
    VmaAllocator vma_allocator;
    QueueFamilyIndices queue_family_indices;
    uint32_t minUniformBufferOffsetAlignment;

#if BB_DEVELOPMENT
    vk::DebugUtilsMessengerEXT debug_messenger;
#endif
};

VulkanContext::VulkanContext(SDL_Window* window)
{
    m_impl = std::make_unique<Impl>();
    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    auto loader_version = vk::enumerateInstanceVersion().value;
    if (loader_version < VULKAN_API_VERSION)
    {
        spdlog::error("VulkanContext: Requires version {}.{}, but max supported by loader is {}.{}",
            vk::apiVersionMajor(VULKAN_API_VERSION),
            vk::apiVersionMinor(VULKAN_API_VERSION),
            vk::apiVersionMajor(loader_version),
            vk::apiVersionMinor(loader_version),
            loader_version);

        assert(false);
        return;
    }

    auto layers = vk::enumerateInstanceLayerProperties().value;

    std::vector<std::string_view> layer_names;
    for (const auto& layer : layers)
    {
        layer_names.push_back(std::string_view(layer.layerName));
    }

#if BB_DEVELOPMENT
    if (std::ranges::find(layer_names, VALIDATION_LAYER) == layer_names.end())
    {
        spdlog::error("VulkanContext: Requested layer {} not supported!", VALIDATION_LAYER);
        assert(false);
        return;
    }
#endif

    std::vector<const char*> requested_extensions;

#if BB_DEVELOPMENT || BB_PLATFORM == BB_LINUX
    requested_extensions.insert(requested_extensions.end(), std::begin(INSTANCE_EXTENSIONS), std::end(INSTANCE_EXTENSIONS));
#endif

    uint32_t sdl_extensions_count = 0;
    const char* const* extension_array = SDL_Vulkan_GetInstanceExtensions(&sdl_extensions_count);
    requested_extensions.insert(requested_extensions.end(), extension_array, extension_array + sdl_extensions_count);

    auto instance_extensions = vk::enumerateInstanceExtensionProperties().value;

    std::vector<std::string_view> extension_names;
    for (const auto& extension : instance_extensions)
    {
        extension_names.push_back(std::string_view(extension.extensionName));
    }

    for (std::string_view extension : requested_extensions)
    {
        if (std::ranges::find(extension_names, extension) == extension_names.end())
        {
            spdlog::error("VulkanContext: Requested instance extension {} not supported!", extension);
            assert(false);
            return;
        }
    }

    vk::ApplicationInfo appInfo {
        .pApplicationName = "Blightspire",
        .applicationVersion = vk::makeApiVersion(0, 0, 0, 0),
        .pEngineName = "Blightspire Engine",
        .engineVersion = vk::makeApiVersion(0, 1, 0, 0),
        .apiVersion = VULKAN_API_VERSION,
    };

    vk::InstanceCreateInfo instance_info {};
    instance_info.flags = vk::InstanceCreateFlags {};
    instance_info.pApplicationInfo = &appInfo;

    instance_info.enabledExtensionCount = static_cast<uint32_t>(requested_extensions.size());
    instance_info.ppEnabledExtensionNames = requested_extensions.data(); // Extensions.

#if BB_DEVELOPMENT
    instance_info.enabledLayerCount = 1;
    instance_info.ppEnabledLayerNames = &VALIDATION_LAYER;
#endif

#if BB_DEVELOPMENT
    vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_info {};
    {
        using enum vk::DebugUtilsMessageTypeFlagBitsEXT;
        using enum vk::DebugUtilsMessageSeverityFlagBitsEXT;
        debug_messenger_info.messageSeverity = eVerbose | eInfo | eWarning | eError;
        debug_messenger_info.messageType = eGeneral | eValidation | ePerformance;
        debug_messenger_info.pfnUserCallback = VulkanDebugCallback;
        debug_messenger_info.pUserData = nullptr;
    }

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> create_info;
    create_info.assign(debug_messenger_info);
    create_info.assign(instance_info);
#else
    vk::StructureChain<vk::InstanceCreateInfo> create_info {};
    create_info.assign(instance_info);
#endif

    util::VK_ASSERT(vk::createInstance(&create_info.get(), nullptr, &m_impl->instance), "Failed to create vk instance!");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_impl->instance);

#if BB_DEVELOPMENT
    util::VK_ASSERT(m_impl->instance.createDebugUtilsMessengerEXT(&debug_messenger_info, nullptr, &m_impl->debug_messenger),
        "Failed to create debug messenger!");
#endif

    VkSurfaceKHR window_surface = nullptr;
    if (!SDL_Vulkan_CreateSurface(window, m_impl->instance, nullptr, &window_surface))
    {
        spdlog::error("VulkanContext: Failed creating SDL vk::Surface. {}", SDL_GetError());
        assert(false);
        return;
    }
    m_impl->surface = window_surface;

    std::vector<vk::PhysicalDevice> devices = m_impl->instance.enumeratePhysicalDevices().value;
    if (devices.empty())
        throw std::runtime_error("No GPU's with Vulkan support available!");

    std::multimap<int, vk::PhysicalDevice> candidates {};

    for (const auto& device : devices)
    {
        uint32_t score = RateDeviceSuitability(device, m_impl->surface);
        if (score > 0)
            candidates.emplace(score, device);
    }
    if (candidates.empty())
        throw std::runtime_error("Failed finding suitable device!");

    m_impl->physical_device = candidates.rbegin()->second;

    m_impl->queue_family_indices = FindQueueFamilies(m_impl->physical_device, m_impl->surface);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos {};
    std::set<uint32_t> uniqueQueueFamilies = { m_impl->queue_family_indices.graphicsFamily.value(), m_impl->queue_family_indices.presentFamily.value() };
    float queuePriority { 1.0f };

    for (uint32_t familyQueueIndex : uniqueQueueFamilies)
        queueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo { .flags = vk::DeviceQueueCreateFlags {}, .queueFamilyIndex = familyQueueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority });

    vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceDynamicRenderingFeaturesKHR, vk::PhysicalDeviceDescriptorIndexingFeatures, vk::PhysicalDeviceSynchronization2Features> structureChain;

    // TODO: this section needs to be revised
    // getFeatures2 already fills in the structureChain with ALL supported features
    // setting them manually after this is redundant and at worse, enables features that are not supported
    // Since we are not doing something crazy like conditionally rendering based on features
    // its probably best to filter devices based on their supported features in RateDeviceSuitability

    auto& deviceFeatures = structureChain.get<vk::PhysicalDeviceFeatures2>();
    m_impl->physical_device.getFeatures2(&deviceFeatures);

    auto& synchronization2Features = structureChain.get<vk::PhysicalDeviceSynchronization2Features>();
    synchronization2Features.synchronization2 = true;

    auto& indexingFeatures = structureChain.get<vk::PhysicalDeviceDescriptorIndexingFeatures>();
    indexingFeatures.runtimeDescriptorArray = true;
    indexingFeatures.descriptorBindingPartiallyBound = true;
    indexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = vk::True;

    auto& dynamicRenderingFeaturesKhr = structureChain.get<vk::PhysicalDeviceDynamicRenderingFeaturesKHR>();
    dynamicRenderingFeaturesKhr.dynamicRendering = true;

    auto& createInfo = structureChain.get<vk::DeviceCreateInfo>();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = nullptr;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
    createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

    util::VK_ASSERT(m_impl->physical_device.createDevice(&createInfo, nullptr, &m_impl->device), "Failed creating a logical device!");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_impl->device);

    m_impl->device.getQueue(m_impl->queue_family_indices.graphicsFamily.value(), 0, &m_impl->graphics_queue);
    m_impl->device.getQueue(m_impl->queue_family_indices.presentFamily.value(), 0, &m_impl->present_queue);

    vk::CommandPoolCreateInfo commandPoolCreateInfo {};
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    commandPoolCreateInfo.queueFamilyIndex = m_impl->queue_family_indices.graphicsFamily.value();

    util::VK_ASSERT(m_impl->device.createCommandPool(&commandPoolCreateInfo, nullptr, &m_impl->command_pool), "Failed creating command pool!");

    std::vector<vk::DescriptorPoolSize> poolSizes = {
        { vk::DescriptorType::eSampler, 1024 },
        { vk::DescriptorType::eCombinedImageSampler, 1024 },
        { vk::DescriptorType::eSampledImage, 1024 },
        { vk::DescriptorType::eStorageImage, 1024 },
        { vk::DescriptorType::eUniformTexelBuffer, 1024 },
        { vk::DescriptorType::eStorageTexelBuffer, 1024 },
        { vk::DescriptorType::eUniformBuffer, 1024 },
        { vk::DescriptorType::eStorageBuffer, 1024 },
        { vk::DescriptorType::eUniformBufferDynamic, 1024 },
        { vk::DescriptorType::eStorageBufferDynamic, 1024 },
        { vk::DescriptorType::eInputAttachment, 1024 }
    };

    vk::DescriptorPoolCreateInfo pool_info {};
    pool_info.poolSizeCount = poolSizes.size();
    pool_info.pPoolSizes = poolSizes.data();
    pool_info.maxSets = 256;
    pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
    util::VK_ASSERT(m_impl->device.createDescriptorPool(&pool_info, nullptr, &m_impl->descriptor_pool), "Failed creating descriptor pool!");

    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo vmaAllocatorCreateInfo {};
    vmaAllocatorCreateInfo.physicalDevice = m_impl->physical_device;
    vmaAllocatorCreateInfo.device = m_impl->device;
    vmaAllocatorCreateInfo.instance = m_impl->instance;
    vmaAllocatorCreateInfo.vulkanApiVersion = vk::makeApiVersion(0, 1, 4, 0);
    vmaAllocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
    vmaCreateAllocator(&vmaAllocatorCreateInfo, &m_impl->vma_allocator);

    vk::PhysicalDeviceProperties properties;
    m_impl->physical_device.getProperties(&properties);
    m_impl->minUniformBufferOffsetAlignment = properties.limits.minUniformBufferOffsetAlignment;

    spdlog::info("##### SYSTEM INFO #####");
    spdlog::info("Operating System: {}", bb::getOsName());

    uint32_t apiVersion = vk::enumerateInstanceVersion().value;
    uint32_t major = VK_VERSION_MAJOR(apiVersion);
    uint32_t minor = VK_VERSION_MINOR(apiVersion);
    uint32_t patch = VK_VERSION_PATCH(apiVersion);
    spdlog::info("Vulkan Version Installed: {}.{}.{}", major, minor, patch);

    spdlog::info("GPU: {}", std::string(properties.deviceName));
    spdlog::info("GPU Driver Version (encoded): {}", properties.driverVersion); // Encoding can be different for each vendor
    spdlog::info("#######################");
}

VulkanContext::~VulkanContext()
{
#if BB_DEVELOPMENT
    if (m_impl->debug_messenger)
        m_impl->instance.destroyDebugUtilsMessengerEXT(m_impl->debug_messenger, nullptr);
#endif

    m_impl->device.destroy(m_impl->descriptor_pool);
    m_impl->device.destroy(m_impl->command_pool);

    vmaDestroyAllocator(m_impl->vma_allocator);

    m_impl->instance.destroy(m_impl->surface);
    m_impl->device.destroy();
    m_impl->instance.destroy();
}

VkInstance VulkanContext::Instance() const { return m_impl->instance; }
VkPhysicalDevice VulkanContext::PhysicalDevice() const { return m_impl->physical_device; }
VkDevice VulkanContext::Device() const { return m_impl->device; }
VkQueue VulkanContext::GraphicsQueue() const { return m_impl->graphics_queue; }
VkQueue VulkanContext::PresentQueue() const { return m_impl->present_queue; }
VkSurfaceKHR VulkanContext::Surface() const { return m_impl->surface; }
VkDescriptorPool VulkanContext::DescriptorPool() const { return m_impl->descriptor_pool; }
VkCommandPool VulkanContext::CommandPool() const { return m_impl->command_pool; }
VmaAllocator VulkanContext::MemoryAllocator() const { return m_impl->vma_allocator; }
const QueueFamilyIndices& VulkanContext::QueueFamilies() const { return m_impl->queue_family_indices; }

void VulkanContext::DebugSetObjectName(void* vulkanObject, uint32_t objectType, const char* name) const
{
#if BB_DEVELOPMENT
    if (name == nullptr)
    {
        return;
    }

    vk::DebugUtilsObjectNameInfoEXT name_info {
        .objectType = static_cast<vk::ObjectType>(objectType),
        .objectHandle = std::bit_cast<uint64_t>(vulkanObject),
        .pObjectName = name
    };

    auto result = m_impl->device.setDebugUtilsObjectNameEXT(&name_info);
    util::VK_ASSERT(result, "Failed to name object!");
#else
    (void)vulkanObject;
    (void)objectType;
    (void)name;
#endif
}
