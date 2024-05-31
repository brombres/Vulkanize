#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
//  Descriptor
//==============================================================================
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

vkb::DispatchTable Descriptor::device_dispatch()
{
  return context->device_dispatch;
}

uint32_t Descriptor::swapchain_count()
{
  return context->swapchain_count;
}

//==============================================================================
//  Descriptors
//==============================================================================
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

//Descriptor* Descriptors::add_combined_image_sampler( uint32_t binding, VkShaderStageFlags stage,
//    VkSampler* samplers, uint32_t count )
//{
//  return add_descriptor(
//    binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage, samplers, count
//  );
//}
//
//Descriptor* Descriptors::add_descriptor( uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage,
//    VkSampler* samplers, uint32_t count )
//{
//  Descriptor* info = new Descriptor();
//  info->binding  = binding;
//  info->count    = count;
//  info->type     = type;
//  info->stage    = stage;
//  info->samplers = samplers;
//  descriptor_info.push_back( info );
//  return info;
//}

bool Descriptors::collect_descriptor_writes( uint32_t swap_index, vector<VkWriteDescriptorSet>& writes )
{
  writes.reserve( descriptor_info.size() );
  for (Descriptor* info : descriptor_info)
  {
    if ( !info->collect_descriptor_write( swap_index, sets[swap_index], writes ) ) return false;
  }
  return true;
}

