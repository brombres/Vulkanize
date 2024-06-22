#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
// OldDescriptor
//==============================================================================
OldDescriptor::OldDescriptor( OldDescriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, VkDescriptorType type )
  : descriptors(descriptors), binding(binding), stage(stage), type(type)
{
}

bool OldDescriptor::activate( Context* context )
{
  if (activated) return true;
  this->context = context;
  if ( !on_activate() ) return false;

  activated = true;
  return true;
}

void OldDescriptor::deactivate()
{
  if ( !activated ) return;
  on_deactivate();
  activated = false;
}

VkDescriptorSet OldDescriptor::get_descriptor_set( int swap_index )
{
  return descriptors->sets[swap_index];
}

bool OldDescriptor::update_descriptor_set_if_modified()
{
  uint32_t swap_index = context->swap_index;
  int frame_bit = (1 << swap_index);
  if ( !(update_frames & frame_bit) ) return false;

  update_descriptor_set( swap_index, descriptors->sets[swap_index] );

  update_frames &= ~frame_bit;
  return true;
}

//==============================================================================
// OldSamplerDescriptor
//==============================================================================
OldSamplerDescriptor::OldSamplerDescriptor(
    OldDescriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, Sampler* sampler )
  : OldDescriptor(descriptors, binding, stage, VK_DESCRIPTOR_TYPE_SAMPLER), sampler(sampler)
{
  image_info.sampler = sampler->vk_sampler;
}

void OldSamplerDescriptor::set( Sampler* new_sampler )
{
  sampler = new_sampler;
  image_info.sampler = new_sampler->vk_sampler;
  if (activated)
  {
    uint32_t swap_index = context->swap_index;
    int frame_bit = (1 << swap_index);
    update_frames = ((1 << context->swapchain_count) - 1) & ~frame_bit;
    update_descriptor_set( swap_index, descriptors->sets[swap_index] );
  }
}

bool OldSamplerDescriptor::update_descriptor_set( uint32_t swap_index,
    VkDescriptorSet& set )
{
  VkWriteDescriptorSet write;
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = set;
  write.dstBinding = binding;
  write.dstArrayElement = 0;
  write.descriptorCount = 1;
  write.descriptorType = type;
  write.pImageInfo = &image_info;

  context->device_dispatch.updateDescriptorSets( 1, &write, 0, nullptr );

  return true;
}

//==============================================================================
// OldCombinedImageSamplerDescriptor
//==============================================================================
OldCombinedImageSamplerDescriptor::OldCombinedImageSamplerDescriptor(
    OldDescriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, Image* image,
    Sampler* sampler )
  : OldDescriptor(descriptors, binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER),
    image(image), sampler(sampler)
{
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  if (image)   image_info.imageView = image->vk_view;
  if (sampler) image_info.sampler   = sampler->vk_sampler;
}

void OldCombinedImageSamplerDescriptor::set( Image* new_image )
{
  image = new_image;
  if (new_image) image_info.imageView = new_image->vk_view;
  if (activated)
  {
    uint32_t swap_index = context->swap_index;
    int frame_bit = (1 << swap_index);
    update_frames = ((1 << context->swapchain_count) - 1) & ~frame_bit;
    update_descriptor_set( swap_index, descriptors->sets[swap_index] );
  }
}

void OldCombinedImageSamplerDescriptor::set( Image* new_image, Sampler* new_sampler )
{
  image = new_image;
  sampler = new_sampler;
  if (new_image)   image_info.imageView = new_image->vk_view;
  if (new_sampler) image_info.sampler   = new_sampler->vk_sampler;
  if (activated)
  {
    uint32_t swap_index = context->swap_index;
    int frame_bit = (1 << swap_index);
    update_frames = ((1 << context->swapchain_count) - 1) & ~frame_bit;
    update_descriptor_set( swap_index, descriptors->sets[swap_index] );
  }
}

void OldCombinedImageSamplerDescriptor::set( Sampler* new_sampler )
{
  sampler = new_sampler;
  if (new_sampler) image_info.sampler = new_sampler->vk_sampler;
  if (activated)
  {
    uint32_t swap_index = context->swap_index;
    int frame_bit = (1 << swap_index);
    update_frames = ((1 << context->swapchain_count) - 1) & ~frame_bit;
    update_descriptor_set( swap_index, descriptors->sets[swap_index] );
  }
}

bool OldCombinedImageSamplerDescriptor::update_descriptor_set( uint32_t swap_index,
    VkDescriptorSet& set )
{
  if (image && sampler)
  {
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

  return true;
}

//==============================================================================
// OldSampledImageDescriptor
//==============================================================================
OldSampledImageDescriptor::OldSampledImageDescriptor(
    OldDescriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, Image* image )
  : OldDescriptor(descriptors, binding, stage, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE), image(image)
{
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView   = image->vk_view;
}

void OldSampledImageDescriptor::set( Image* new_image )
{
  image = new_image;
  image_info.imageView = new_image->vk_view;
  if (activated)
  {
    uint32_t swap_index = context->swap_index;
    int frame_bit = (1 << swap_index);
    update_frames = ((1 << context->swapchain_count) - 1) & ~frame_bit;
    update_descriptor_set( swap_index, descriptors->sets[swap_index] );
  }
}

bool OldSampledImageDescriptor::update_descriptor_set( uint32_t swap_index,
    VkDescriptorSet& set )
{
  VkWriteDescriptorSet write;
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = set;
  write.dstBinding = binding;
  write.dstArrayElement = 0;
  write.descriptorCount = 1;
  write.descriptorType = type;
  write.pImageInfo = &image_info;

  context->device_dispatch.updateDescriptorSets( 1, &write, 0, nullptr );

  return true;
}

//==============================================================================
// OldDescriptors
//==============================================================================
OldDescriptors::~OldDescriptors()
{
  for (OldDescriptor* descriptor : descriptors)
  {
    delete descriptor;
  }
  descriptors.clear();
}

bool OldDescriptors::activate( Context* context )
{
  if (activated) return true;
  this->context = context;

  for (auto descriptor : descriptors)
  {
    if ( !descriptor->activate(context) ) return false;
  }

  activated = true;

  return true;
}

void OldDescriptors::deactivate()
{
  if ( !activated ) return;
  for (auto descriptor : descriptors) descriptor->deactivate();
}

OldCombinedImageSamplerDescriptor* OldDescriptors::add_combined_image_sampler(
    uint32_t binding, VkShaderStageFlags stage, Image* image, Sampler* sampler )
{
  OldCombinedImageSamplerDescriptor* descriptor = new OldCombinedImageSamplerDescriptor(
    this, binding, stage, image, sampler
  );
  descriptors.push_back( descriptor );
  return descriptor;
}

OldSampledImageDescriptor* OldDescriptors::add_sampled_image(
    uint32_t binding, VkShaderStageFlags stage, Image* image )
{
  OldSampledImageDescriptor* descriptor = new OldSampledImageDescriptor(
    this, binding, stage, image
  );
  descriptors.push_back( descriptor );
  return descriptor;
}

OldSamplerDescriptor* OldDescriptors::add_sampler(
    uint32_t binding, VkShaderStageFlags stage, Sampler* sampler )
{
  OldSamplerDescriptor* descriptor = new OldSamplerDescriptor( this, binding, stage, sampler );
  descriptors.push_back( descriptor );
  return descriptor;
}

void OldDescriptors::cmd_bind( VkCommandBuffer cmd, GraphicsPipeline* pipeline )
{
  update_modified_descriptor_sets();
  context->device_dispatch.cmdBindDescriptorSets( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline->layout, 0, 1, &sets[context->swap_index], 0, nullptr );
}

bool OldDescriptors::configure_descriptor_sets()
{
  uint32_t swapchain_count = context->swapchain_count;
  for (uint32_t swap_index=0; swap_index<swapchain_count; ++swap_index)
  {
    for (OldDescriptor* descriptor : descriptors)
    {
      if ( !descriptor->update_descriptor_set(swap_index, sets[swap_index]) ) return false;
    }
  }
  return true;
}

bool OldDescriptors::update_modified_descriptor_sets()
{
  bool any_updated = false;
  for (OldDescriptor* descriptor : descriptors)
  {
    if (descriptor->update_descriptor_set_if_modified()) any_updated = true;
  }
  return any_updated;
}

