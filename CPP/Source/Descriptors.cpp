#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

Descriptors::~Descriptors()
{
  for (Descriptor* info : descriptor_info)
  {
    delete info;
  }
  descriptor_info.clear();
}

Descriptor* Descriptors::add_combined_image_sampler( uint32_t binding, VkShaderStageFlags stage,
    VkSampler* samplers, uint32_t count )
{
  return add_descriptor(
    binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage, samplers, count
  );
}

Descriptor* Descriptors::add_descriptor( uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage,
    VkSampler* samplers, uint32_t count )
{
  Descriptor* info = new Descriptor();
  info->binding  = binding;
  info->count    = count;
  info->type     = type;
  info->stage    = stage;
  info->samplers = samplers;
  descriptor_info.push_back( info );
  return info;
}

