#pragma once

#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

namespace VKZ
{
  struct Context;

  struct Image
  {
    Context*       context;
    VkFormat       format;
    VkImage        image;
    VkDeviceMemory memory;
    VkImageView    view;
    bool           image_created = false;
    bool           memory_allocated = false;
    bool           view_created = false;
    bool           exists = false;

    Image();
    Image( Context* context, int width, int height, VkFormat format,
           VkImageUsageFlags usage, VkImageAspectFlags aspect,
           VkMemoryPropertyFlags properties=VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
           VkImageTiling tiling=VK_IMAGE_TILING_OPTIMAL );
    ~Image();

    bool create( Context* context, int width, int height, VkFormat format,
                 VkImageUsageFlags usage, VkImageAspectFlags aspect,
                 VkMemoryPropertyFlags properties=VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 VkImageTiling tiling=VK_IMAGE_TILING_OPTIMAL );
    void destroy();
  };
};
