#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
// Descriptor
//==============================================================================

//==============================================================================
// ImageInfoDescriptor
//==============================================================================
void ImageInfoDescriptor::update_descriptor_set( VkDescriptorSet& set, size_t swap_index )
{
  int frame_bit = (1 << swap_index);
  if ( !(update_frames & frame_bit) ) return;
  update_frames &= ~frame_bit;

  VkWriteDescriptorSet write;
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = set;
  write.dstBinding = binding;
  write.dstArrayElement = 0;
  write.descriptorCount = 1;
  write.descriptorType = type;
  write.pImageInfo = &image_info;

  context->device_dispatch.updateDescriptorSets( 1, &write, 0, nullptr );
}

//==============================================================================
// CombinedImageSamplerDescriptor
//==============================================================================
CombinedImageSamplerDescriptor::CombinedImageSamplerDescriptor( Context* context,
    uint32_t binding, VkShaderStageFlags stage, size_t initial_count )
  : ImageInfoDescriptor( context, binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
{
  images.resize( initial_count );
  samplers.resize( initial_count );
  count = (uint32_t)initial_count;
}

CombinedImageSamplerDescriptor::CombinedImageSamplerDescriptor( Context* context,
    uint32_t binding, VkShaderStageFlags stage, Ref<Image> image, Ref<Sampler> sampler )
  : ImageInfoDescriptor( context, binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
{
  images.push_back( image );
  samplers.push_back( sampler );
  count = (uint32_t)images.size();
}

void CombinedImageSamplerDescriptor::add( Ref<Image> image, Ref<Sampler> sampler )
{
  images.push_back( image );
  samplers.push_back( sampler );
  count = (uint32_t)images.size();
}

void CombinedImageSamplerDescriptor::set( size_t index, Ref<Image> image, Ref<Sampler> sampler )
{
  if ( !_validate_index(index) ) return;
  images[index] = image;
  samplers[index] = sampler;
  update_frames = ((1 << context->swapchain_count) - 1);
}

void CombinedImageSamplerDescriptor::set( size_t index, Ref<Image> image )
{
  if ( !_validate_index(index) ) return;
  images[index] = image;
  update_frames = ((1 << context->swapchain_count) - 1);
}

void CombinedImageSamplerDescriptor::set( size_t index, Ref<Sampler> sampler )
{
  if ( !_validate_index(index) ) return;
  samplers[index] = sampler;
  update_frames = ((1 << context->swapchain_count) - 1);
}

bool CombinedImageSamplerDescriptor::_validate_index( size_t index )
{
  if (index < images.size()) return true;
  VKZ_LOG_ERROR( "[Vulkanize] Error in CombinedImageSamplerDescriptor::set() - index out of bounds." );
  return false;
}
