#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
// Descriptor
//==============================================================================
Descriptor::Descriptor( Descriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, VkDescriptorType type )
  : descriptors(descriptors), binding(binding), stage(stage), type(type)
{
}

bool Descriptor::activate( Context* context )
{
  if (activated) return true;
  this->context = context;
  if ( !on_activate() ) return false;

  activated = true;
  return true;
}

void Descriptor::deactivate()
{
  if ( !activated ) return;
  on_deactivate();
  activated = false;
}

VkDescriptorSet Descriptor::get_descriptor_set( int swap_index )
{
  return descriptors->sets[swap_index];
}

bool Descriptor::update_descriptor_set_if_modified()
{
  uint32_t swap_index = context->swap_index;
  int frame_bit = (1 << swap_index);
  if ( !(update_frames & frame_bit) ) return false;

  update_descriptor_set( swap_index, descriptors->sets[swap_index] );

  update_frames &= ~frame_bit;
  return true;
}

//==============================================================================
// SamplerDescriptor
//==============================================================================
SamplerDescriptor::SamplerDescriptor(
    Descriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, Sampler* sampler )
  : Descriptor(descriptors, binding, stage, VK_DESCRIPTOR_TYPE_SAMPLER), sampler(sampler)
{
  image_info.sampler = sampler->vk_sampler;
}

void SamplerDescriptor::set( Sampler* new_sampler )
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

bool SamplerDescriptor::update_descriptor_set( uint32_t swap_index,
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
// CombinedImageSamplerDescriptor
//==============================================================================
CombinedImageSamplerDescriptor::CombinedImageSamplerDescriptor(
    Descriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, Image* image,
    Sampler* sampler )
  : Descriptor(descriptors, binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER),
    image(image), sampler(sampler)
{
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  if (image)   image_info.imageView = image->vk_view;
  if (sampler) image_info.sampler   = sampler->vk_sampler;
}

void CombinedImageSamplerDescriptor::set( Image* new_image )
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

void CombinedImageSamplerDescriptor::set( Image* new_image, Sampler* new_sampler )
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

void CombinedImageSamplerDescriptor::set( Sampler* new_sampler )
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

bool CombinedImageSamplerDescriptor::update_descriptor_set( uint32_t swap_index,
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
// SampledImageDescriptor
//==============================================================================
SampledImageDescriptor::SampledImageDescriptor(
    Descriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, Image* image )
  : Descriptor(descriptors, binding, stage, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE), image(image)
{
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView   = image->vk_view;
}

void SampledImageDescriptor::set( Image* new_image )
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

bool SampledImageDescriptor::update_descriptor_set( uint32_t swap_index,
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
// Descriptors
//==============================================================================
Descriptors::~Descriptors()
{
  for (Descriptor* descriptor : descriptors)
  {
    delete descriptor;
  }
  descriptors.clear();
}

bool Descriptors::activate( Context* context )
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

void Descriptors::deactivate()
{
  if ( !activated ) return;
  for (auto descriptor : descriptors) descriptor->deactivate();
}

CombinedImageSamplerDescriptor* Descriptors::add_combined_image_sampler(
    uint32_t binding, VkShaderStageFlags stage, Image* image, Sampler* sampler )
{
  CombinedImageSamplerDescriptor* descriptor = new CombinedImageSamplerDescriptor(
    this, binding, stage, image, sampler
  );
  descriptors.push_back( descriptor );
  return descriptor;
}

SampledImageDescriptor* Descriptors::add_sampled_image(
    uint32_t binding, VkShaderStageFlags stage, Image* image )
{
  SampledImageDescriptor* descriptor = new SampledImageDescriptor(
    this, binding, stage, image
  );
  descriptors.push_back( descriptor );
  return descriptor;
}

SamplerDescriptor* Descriptors::add_sampler(
    uint32_t binding, VkShaderStageFlags stage, Sampler* sampler )
{
  SamplerDescriptor* descriptor = new SamplerDescriptor( this, binding, stage, sampler );
  descriptors.push_back( descriptor );
  return descriptor;
}

bool Descriptors::configure_descriptor_sets()
{
  uint32_t swapchain_count = context->swapchain_count;
  for (uint32_t swap_index=0; swap_index<swapchain_count; ++swap_index)
  {
    for (Descriptor* descriptor : descriptors)
    {
      if ( !descriptor->update_descriptor_set(swap_index, sets[swap_index]) ) return false;
    }
  }
  return true;
}

bool Descriptors::update_modified_descriptor_sets()
{
  bool any_updated = false;
  for (Descriptor* descriptor : descriptors)
  {
    if (descriptor->update_descriptor_set_if_modified()) any_updated = true;
  }
  return any_updated;
}

