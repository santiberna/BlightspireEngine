
#pragma once

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