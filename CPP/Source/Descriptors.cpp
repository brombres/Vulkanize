#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
// Descriptor
//==============================================================================
Descriptor::Descriptor( uint32_t binding, VkShaderStageFlags stage, VkDescriptorType type )
  : binding(binding), stage(stage), type(type)
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

//==============================================================================
// CombinedImageSamplerDescriptor
//==============================================================================
CombinedImageSamplerDescriptor::CombinedImageSamplerDescriptor(
    uint32_t binding, VkShaderStageFlags stage, Image* image, VkSampler vk_sampler )
  : image(image), vk_sampler(vk_sampler),
    Descriptor(binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
{
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView   = image->vk_view;
  image_info.sampler     = vk_sampler;
}

bool CombinedImageSamplerDescriptor::configure_descriptor_set( uint32_t swap_index,
    VkDescriptorSet& set )
{
  VkWriteDescriptorSet write;
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = set;
  write.dstBinding = binding;
  write.dstArrayElement = 0;
  write.descriptorCount = 1;
  write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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
    uint32_t binding, VkShaderStageFlags stage, Image* image, VkSampler sampler )
{
  CombinedImageSamplerDescriptor* descriptor = new CombinedImageSamplerDescriptor(
    binding, stage, image, sampler
  );
  descriptors.push_back( descriptor );
  return descriptor;
}

bool Descriptors::configure_descriptor_sets( uint32_t swap_index )
{
  for (Descriptor* descriptor : descriptors)
  {
    if ( !descriptor->configure_descriptor_set(swap_index, sets[swap_index]) ) return false;
  }
  return true;
}

