
#pragma once

using VkDeviceSize = bb::u64;

// WARNING: this will completely break on x32 systems (not supported)
#define BB_VULKAN_FWD(handle) \
    struct handle##_T;        \
    using handle = handle##_T*;

BB_VULKAN_FWD(VkInstance);
BB_VULKAN_FWD(VkPhysicalDevice);
BB_VULKAN_FWD(VkDevice);
BB_VULKAN_FWD(VkQueue);
BB_VULKAN_FWD(VkSurfaceKHR);
BB_VULKAN_FWD(VkDescriptorPool);
BB_VULKAN_FWD(VkCommandPool);
BB_VULKAN_FWD(VmaAllocator);
BB_VULKAN_FWD(VmaAllocation);
BB_VULKAN_FWD(VkCommandBuffer);
BB_VULKAN_FWD(VkBuffer);
BB_VULKAN_FWD(VkFence);
BB_VULKAN_FWD(VkImage);
BB_VULKAN_FWD(VkSampler);
BB_VULKAN_FWD(VkDescriptorSetLayout);
BB_VULKAN_FWD(VkDescriptorSet);
