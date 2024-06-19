#pragma once

#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

namespace VKZ
{
  struct Context;

  struct ImageInfo
  {
    // PROPERTIES
    Context*              context;
    VkImageCreateInfo     image_info = {};
    VkMemoryPropertyFlags memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkImageViewCreateInfo view_info = {};

    // METHODS
    ImageInfo( Context* context );

    ImageInfo(
      Context* context,
      int width, int height,
      VkFormat format=VK_FORMAT_R8G8B8A8_SRGB,
      VkImageUsageFlags usage=VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VkImageAspectFlags aspect=VK_IMAGE_ASPECT_COLOR_BIT,
      VkMemoryPropertyFlags memory_properties=VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      VkImageTiling tiling=VK_IMAGE_TILING_OPTIMAL
     );

    virtual void configure(
      int width, int height,
      VkFormat format=VK_FORMAT_R8G8B8A8_SRGB,
      VkImageUsageFlags usage=VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VkImageAspectFlags aspect=VK_IMAGE_ASPECT_COLOR_BIT,
      VkMemoryPropertyFlags memory_properties=VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      VkImageTiling tiling=VK_IMAGE_TILING_OPTIMAL
    );
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
    Image( ImageInfo& info );
    Image( Context* context, void* pixel_data, int width, int height );
    ~Image();

    virtual bool create( ImageInfo& info );
    virtual bool create( Context* context, void* pixel_data, int width, int height );
    virtual void destroy();

    virtual void cmd_wait_until_drawing_complete( VkCommandBuffer cmd );
    virtual void copy_from( Buffer& buffer );
    virtual void copy_from( void* pixel_data );

    virtual void transition_layout( VkImageLayout new_layout );
  };
};
