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
  write.descriptorType = type;
  write.descriptorCount = (uint32_t)image_infos.size();
  write.pImageInfo = image_infos.data();

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
  image_infos.resize( initial_count );
  for (auto& info : image_infos)
  {
    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }
  count = (uint32_t)initial_count;
}

CombinedImageSamplerDescriptor::CombinedImageSamplerDescriptor( Context* context,
    uint32_t binding, VkShaderStageFlags stage, Ref<Image> image, Ref<Sampler> sampler )
  : ImageInfoDescriptor( context, binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
{
  add( image, sampler );
}

void CombinedImageSamplerDescriptor::add( Ref<Image> image, Ref<Sampler> sampler )
{
  if ( !sampler.exists() ) sampler = Sampler::create_default( context );

  images.push_back( image );
  samplers.push_back( sampler );

  VkDescriptorImageInfo info = {};
  info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  if (image.exists())   info.imageView = image->vk_view;
  if (sampler.exists()) info.sampler   = sampler->vk_sampler;
  image_infos.push_back( info );

  count = (uint32_t)images.size();
  update_frames = ((1 << context->swapchain_count) - 1);
}

void CombinedImageSamplerDescriptor::set( size_t index, Ref<Image> image, Ref<Sampler> sampler )
{
  if ( !_validate_index(index) ) return;

  if ( !sampler.exists() ) sampler = Sampler::create_default( context );

  images[index] = image;
  samplers[index] = sampler;
  if (image.exists())   image_infos[index].imageView = image->vk_view;
  if (sampler.exists()) image_infos[index].sampler   = sampler->vk_sampler;
  update_frames = ((1 << context->swapchain_count) - 1);
}

void CombinedImageSamplerDescriptor::set_image( size_t index, Ref<Image> image )
{
  if ( !_validate_index(index) ) return;
  images[index] = image;
  if (image.exists())   image_infos[index].imageView = image->vk_view;
  update_frames = ((1 << context->swapchain_count) - 1);
}

void CombinedImageSamplerDescriptor::set_sampler( size_t index, Ref<Sampler> sampler )
{
  if ( !_validate_index(index) ) return;
  samplers[index] = sampler;
  if (sampler.exists()) image_infos[index].sampler   = sampler->vk_sampler;
  update_frames = ((1 << context->swapchain_count) - 1);
}

bool CombinedImageSamplerDescriptor::_validate_index( size_t index )
{
  if (index < images.size()) return true;
  VKZ_LOG_ERROR( "[Vulkanize] Error: index out of bounds in CombinedImageSamplerDescriptor::set()." );
  return false;
}

//==============================================================================
// SampledImageDescriptor
//==============================================================================
SampledImageDescriptor::SampledImageDescriptor( Context* context,
    uint32_t binding, VkShaderStageFlags stage, size_t initial_count )
  : ImageInfoDescriptor( context, binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
{
  images.resize( initial_count );
  image_infos.resize( initial_count );
  for (auto& info : image_infos)
  {
    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }
  count = (uint32_t)initial_count;
}

SampledImageDescriptor::SampledImageDescriptor( Context* context,
    uint32_t binding, VkShaderStageFlags stage, Ref<Image> image )
  : ImageInfoDescriptor( context, binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
{
  add( image );
}

void SampledImageDescriptor::add( Ref<Image> image )
{
  images.push_back( image );

  VkDescriptorImageInfo info = {};
  info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  if (image.exists()) info.imageView = image->vk_view;
  image_infos.push_back( info );

  count = (uint32_t)images.size();
  update_frames = ((1 << context->swapchain_count) - 1);
}

void SampledImageDescriptor::set( size_t index, Ref<Image> image )
{
  if (index >= images.size())
  {
    VKZ_LOG_ERROR( "[Vulkanize] Error: index out of bounds in SampledImageDescriptor::set()." );
    return;
  }

  images[index] = image;
  if (image.exists()) image_infos[index].imageView = image->vk_view;
  update_frames = ((1 << context->swapchain_count) - 1);
}

//==============================================================================
// SamplerDescriptor
//==============================================================================
SamplerDescriptor::SamplerDescriptor( Context* context,
    uint32_t binding, VkShaderStageFlags stage, size_t initial_count )
  : ImageInfoDescriptor( context, binding, stage, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE )
{
  samplers.resize( initial_count );
  image_infos.resize( initial_count );
  for (auto& info : image_infos)
  {
    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }
  count = (uint32_t)initial_count;
}

SamplerDescriptor::SamplerDescriptor( Context* context,
    uint32_t binding, VkShaderStageFlags stage, Ref<Sampler> sampler )
  : ImageInfoDescriptor( context, binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
{
  add( sampler );
}

void SamplerDescriptor::add( Ref<Sampler> sampler )
{
  samplers.push_back( sampler );

  VkDescriptorImageInfo info = {};
  info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  if (sampler.exists()) info.sampler = sampler->vk_sampler;
  image_infos.push_back( info );

  count = (uint32_t)samplers.size();
  update_frames = ((1 << context->swapchain_count) - 1);
}

void SamplerDescriptor::set( size_t index, Ref<Sampler> sampler )
{
  if (index >= samplers.size())
  {
    VKZ_LOG_ERROR( "[Vulkanize] Error: index out of bounds in SamplerDescriptor::set()." );
    return;
  }

  samplers[index] = sampler;
  if (sampler.exists()) image_infos[index].sampler = sampler->vk_sampler;
  update_frames = ((1 << context->swapchain_count) - 1);
}
