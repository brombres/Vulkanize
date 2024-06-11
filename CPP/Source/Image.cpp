#include "Vulkanize/Vulkanize.h"

using namespace VKZ;

//==============================================================================
// ImageInfo
//==============================================================================
ImageInfo::ImageInfo( Context* context )
{
  this->context = context;
  configure(
    0, 0, VK_FORMAT_R8G8B8A8_SRGB,
    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    VK_IMAGE_TILING_OPTIMAL
  );
}

ImageInfo::ImageInfo( Context* context, int width, int height, VkFormat format,
                      VkImageUsageFlags usage, VkImageAspectFlags aspect,
                      VkMemoryPropertyFlags memory_properties, VkImageTiling tiling )
{
  this->context = context;
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

Image::Image( ImageInfo& info )
{
  create( info );
}

Image::Image( Context* context, void* pixel_data, int width, int height )
{
  create( context, pixel_data, width, height );
}

Image::~Image()
{
  destroy();
}

bool Image::create( ImageInfo& info )
{
  this->context = info.context;
  this->format  = info.image_info.format;
  this->layout  = info.image_info.initialLayout;
  this->width   = info.image_info.extent.width;
  this->height  = info.image_info.extent.height;

  VKZ_ATTEMPT(
    "creating image",
    context->device_dispatch.createImage( &info.image_info, nullptr, &vk_image ),
    return false;
  )
  image_created = true;

  VkMemoryRequirements memory_requirements;
  context->device_dispatch.getImageMemoryRequirements( vk_image, &memory_requirements );

  int memory_type_index = context->find_memory_type(
    memory_requirements.memoryTypeBits, info.memory_properties
  );

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = memory_requirements.size;
  alloc_info.memoryTypeIndex = memory_type_index;

  VKZ_ATTEMPT(
    "allocating image memory",
    context->device_dispatch.allocateMemory( &alloc_info, nullptr, &vk_memory ),
    destroy();
    return false;
  );
  memory_allocated = true;

  context->device_dispatch.bindImageMemory( vk_image, vk_memory, 0 );

  info.view_info.image = vk_image;
  VKZ_ATTEMPT(
    "creating image view",
    context->device_dispatch.createImageView( &info.view_info, nullptr, &vk_view ),
    destroy();
    return false;
  );
  view_created = true;

  exists = true;
  return true;
}

bool Image::create( Context* context, void* pixel_data, int width, int height )
{
  ImageInfo info( context, width, height );
  if ( !create(info) ) return false;

  copy_from( pixel_data );
  return true;
}

void Image::destroy()
{
  if (view_created)
  {
    view_created = false;
    context->device_dispatch.destroyImageView( vk_view, nullptr );
  }

  if (memory_allocated)
  {
    memory_allocated = false;
    context->device_dispatch.freeMemory( vk_memory, nullptr );
  }

  if (image_created)
  {
    image_created = false;
    context->device_dispatch.destroyImage( vk_image, nullptr );
  }

  exists = false;
}

void Image::copy_from( Buffer& buffer )
{
  VkCommandBuffer cmd = context->begin_cmd();

  VkBufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = { 0, 0, 0 };
  region.imageExtent = { width, height, 1 };

  context->device_dispatch.cmdCopyBufferToImage(
    cmd,
    buffer.vk_buffer,
    vk_image,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    1,
    &region
  );

  context->end_cmd( cmd );
}

void Image::copy_from( void* pixel_data )
{
  Buffer staging_buffer;
  staging_buffer.create_staging_buffer( context, 4, width*height );
  staging_buffer.copy_from( pixel_data, width*height );

  transition_layout( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
  copy_from( staging_buffer );
  transition_layout( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

  staging_buffer.destroy();
}

void Image::transition_layout( VkImageLayout new_layout )
{
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = vk_image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags src_stage;
  VkPipelineStageFlags dest_stage;

  if (layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
  {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dest_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  else if (layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
           new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
  {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dest_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  else
  {
    VKZ_LOG_ERROR( "[Vulkanize] Error - unsupported layout transition." );
    return;
  }

  VkCommandBuffer cmd = context->begin_cmd();

  context->device_dispatch.cmdPipelineBarrier(
    cmd,
    src_stage, dest_stage,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
  );

  context->end_cmd( cmd );

  layout = new_layout;
}
