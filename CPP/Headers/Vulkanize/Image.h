#pragma once

#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

namespace VKZ
{
  struct Context;

  struct ImageInfo
  {
    // PROPERTIES
    VkImageCreateInfo     image_info = {};
    VkMemoryPropertyFlags memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkImageViewCreateInfo view_info = {};

    // METHODS
    ImageInfo();

    ImageInfo( int width, int height, VkFormat format,
               VkImageUsageFlags usage, VkImageAspectFlags aspect,
               VkMemoryPropertyFlags memory_properties=VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
               VkImageTiling tiling=VK_IMAGE_TILING_OPTIMAL );

    virtual void configure( int width, int height, VkFormat format,
                            VkImageUsageFlags usage, VkImageAspectFlags aspect,
                            VkMemoryPropertyFlags memory_properties=VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            VkImageTiling tiling=VK_IMAGE_TILING_OPTIMAL );
  };

  struct Image
  {
    Context*       context;
    VkFormat       format;
    VkImageLayout  layout;
    uint32_t       width;
    uint32_t       height;

    VkImage        vk_image;
    VkDeviceMemory vk_memory;
    VkImageView    vk_view;

    bool           image_created = false;
    bool           memory_allocated = false;
    bool           view_created = false;
    bool           exists = false;

    Image();
    Image( Context* context, ImageInfo& info );
    ~Image();

    virtual bool create( Context* context, ImageInfo& info );
    virtual void destroy();

    virtual void copy_from( Buffer& buffer );

    virtual void transition_layout( VkImageLayout new_layout );
  };
};
