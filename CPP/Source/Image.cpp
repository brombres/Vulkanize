#include "Vulkanize/Vulkanize.h"

using namespace VKZ;

//==============================================================================
// ImageInfo
//==============================================================================
ImageInfo::ImageInfo()
{
  configure(
    0, 0, VK_FORMAT_R8G8B8A8_SRGB,
    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    VK_IMAGE_TILING_OPTIMAL
  );
}

ImageInfo::ImageInfo( int width, int height, VkFormat format,
                      VkImageUsageFlags usage, VkImageAspectFlags aspect,
                      VkMemoryPropertyFlags memory_properties, VkImageTiling tiling )
{
  configure( width, height, format, usage, aspect, memory_properties, tiling );
}

void ImageInfo::configure( int width, int height, VkFormat format,
                           VkImageUsageFlags usage, VkImageAspectFlags aspect,
                           VkMemoryPropertyFlags memory_properties, VkImageTiling tiling )
{
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  this->memory_properties = memory_properties;

  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format;
  view_info.subresourceRange.aspectMask = aspect;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;
}

//==============================================================================
// Image
//==============================================================================
Image::Image() {}

Image::Image( Context* context, ImageInfo& info )
{
  create( context, info );
}

Image::~Image()
{
  destroy();
}

bool Image::create( Context* context, ImageInfo& info )
{
  this->context = context;
  this->format = info.image_info.format;

  VKZ_ATTEMPT(
    "creating image",
    context->device_dispatch.createImage( &info.image_info, nullptr, &image ),
    return false;
  )
  image_created = true;

  VkMemoryRequirements memory_requirements;
  context->device_dispatch.getImageMemoryRequirements( image, &memory_requirements );

  int memory_type_index = context->find_memory_type(
    memory_requirements.memoryTypeBits, info.memory_properties
  );

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = memory_requirements.size;
  alloc_info.memoryTypeIndex = memory_type_index;

  VKZ_ATTEMPT(
    "allocating image memory",
    context->device_dispatch.allocateMemory( &alloc_info, nullptr, &memory ),
    destroy();
    return false;
  );
  memory_allocated = true;

  context->device_dispatch.bindImageMemory( image, memory, 0 );

  info.view_info.image = image;
  VKZ_ATTEMPT(
    "creating image view",
    context->device_dispatch.createImageView( &info.view_info, nullptr, &view ),
    destroy();
    return false;
  );
  view_created = true;

  exists = true;
  return true;
}

void Image::destroy()
{
  if (view_created)
  {
    view_created = false;
    context->device_dispatch.destroyImageView( view, nullptr );
  }

  if (memory_allocated)
  {
    memory_allocated = false;
    context->device_dispatch.freeMemory( memory, nullptr );
  }

  if (image_created)
  {
    image_created = false;
    context->device_dispatch.destroyImage( image, nullptr );
  }

  exists = false;
}

