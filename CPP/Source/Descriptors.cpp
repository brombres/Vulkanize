#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

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

bool Descriptor::collect_descriptor_write( std::vector<VkWriteDescriptorSet>& writes )
{
  /*
  if (type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
  {
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView;
    imageInfo.sampler = textureSampler;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].pImageInfo = &imageInfo;

    return true;
  }
  else
  */
  {
    VKZ_LOG_ERROR( "[Vulkanize] Internal error - unhandled type in Descriptor::collect_descriptor_write().\n" );
    return false;
  }
}

uint32_t Descriptor::swapchain_count()
{
  return context->swapchain_count;
}

Descriptors::~Descriptors()
{
  for (Descriptor* info : descriptor_info)
  {
    delete info;
  }
  descriptor_info.clear();
}

bool Descriptors::activate( Context* context )
{
  if (activated) return true;
  this->context = context;

  for (auto descriptor : descriptor_info)
  {
    if ( !descriptor->activate(context) ) return false;
  }

  activated = true;

  return true;
}

void Descriptors::deactivate()
{
  if ( !activated ) return;
  for (auto descriptor : descriptor_info) descriptor->deactivate();
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

